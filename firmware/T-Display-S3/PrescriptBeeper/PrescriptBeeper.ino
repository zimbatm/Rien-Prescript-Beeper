#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "storage.h"
#include "buzzer.h"
#include "serial_debug.h"
#include "buttons.h"
#include "ble_handler.h"

bool pendingDisplayUpdate = false;
bool isDisplaying = false;
bool isInfinite = false;
unsigned long displayStartTime = 0;
unsigned long displayDurationMs = 0;
String currentDisplayText = "";

bool simulatePass = false;
bool simulateFail = false;
bool timerTimeoutFail = false;
bool hasShownPrescript = false;
bool requiresResponse = true;
long lastTickSecond = -1;

Button touchBtn;
Button keyBtn;
Button bootBtn;

unsigned long nextAutoMissionAt = 0;
void drawNewPrescript();

void armAutoMission(unsigned long minutesMin, unsigned long minutesMax) {
  if (minutesMax == 0) {
    nextAutoMissionAt = 0;
    return;
  }
  unsigned long minutes = random(minutesMin, minutesMax + 1);
  nextAutoMissionAt = millis() + minutes * 60000UL;
  if (nextAutoMissionAt == 0) nextAutoMissionAt = 1;
}

void handleAutoMission() {
  if (nextAutoMissionAt == 0) return;
  if ((long)(millis() - nextAutoMissionAt) < 0) return;
  if (isDisplaying) {
    nextAutoMissionAt = millis() + 60000UL;
    return;
  }
  drawNewPrescript();
}

void showPrescriptOnDisplay(const char* text, unsigned long durationMs, bool infinite, bool respond) {
  currentDisplayText = String(text);
  isDisplaying = true;
  isInfinite = infinite;
  displayDurationMs = durationMs;
  requiresResponse = respond;
  lastTickSecond = -1;
  textSoundsActive = false;

  if (!isSystemText(text)) {
    addStats("0,0,1");
    textSoundsActive = true;
    armAutoMission(AUTO_MISSION_MIN_MINUTES, AUTO_MISSION_MAX_MINUTES);
  } else if (currentDisplayText == "CLEAR." || currentDisplayText.startsWith("Volume")) {
    beepPass();
  } else if (currentDisplayText == "FAILED.") {
    beepFail();
  } else if (currentDisplayText == "Connected." || currentDisplayText.startsWith("PIN: ")) {
    beepConnected();
  } else if (currentDisplayText == "Disconnected." || currentDisplayText == "NO DATA") {
    beepDisconnected();
  }

  beginScramble(text);

  displayStartTime = millis();
  hasShownPrescript = true;
}

bool awaitingResponse() {
  return isDisplaying && requiresResponse && !isSystemText(currentDisplayText.c_str());
}

void handleDisplayTimer() {
  if (!hasShownPrescript) return;
  if (!isDisplaying || isInfinite || displayScrambling) return;

  unsigned long elapsed = millis() - displayFinishedTime;
  if (elapsed < displayDurationMs) {
    unsigned long remaining = displayDurationMs - elapsed;
    updateTimerDisplay(remaining);
    if (awaitingResponse()) {
      long sec = (remaining + 999) / 1000;
      if (sec <= COUNTDOWN_TICK_SECONDS && sec != lastTickSecond) {
        lastTickSecond = sec;
        beepTick();
      }
    }
    return;
  }

  if (awaitingResponse()) {
    timerTimeoutFail = true;
  } else {
    isDisplaying = false;
    currentDisplayText = "";
    displayIdle();
  }
}

void drawNewPrescript() {
  int idx = -1;
  String line = getRandomPrescript(&idx);
  if (line.length() == 0) {
    showPrescriptOnDisplay("NO DATA", 1000, false, false);
    return;
  }
  int dur = 10;
  bool respond = true;
  String text = "";
  if (!parsePrescriptLine(line, dur, respond, text)) return;
  showPrescriptOnDisplay(text.c_str(), dur * 1000UL, dur == 0, respond);
  if (bleNotifyReady() && idx >= 0) {
    String evt = "EVT:PRESCRIPT|IDX:" + String(idx) + "\n";
    sendChunked(evt.c_str(), evt.length());
  }
}

void handlePass(bool fromWeb) {
  if (!awaitingResponse() && !fromWeb) {
    drawNewPrescript();
    return;
  }
  if (requiresResponse && currentDisplayText != "CLEAR." && currentDisplayText != "FAILED.") {
    addStats("1,0,0");
    if (bleNotifyReady() && !fromWeb) {
      String res = "EVT:PASS\n";
      sendChunked(res.c_str(), res.length());
    }
    showPrescriptOnDisplay("CLEAR.", 800, false, false);
  }
}

void handleFail(bool fromWeb, bool wasTimeout) {
  if (!isDisplaying && !fromWeb && !wasTimeout) return;
  if (requiresResponse && currentDisplayText != "CLEAR." && currentDisplayText != "FAILED.") {
    addStats("0,1,0");
    if (bleNotifyReady() && !fromWeb) {
      String res = "EVT:FAIL\n";
      sendChunked(res.c_str(), res.length());
    }
    showPrescriptOnDisplay("FAILED.", 800, false, false);
  } else if (!requiresResponse && wasTimeout) {
    displayIdle();
  }
}

void cycleVolume() {
  cycleBuzzerVolume();
  if (awaitingResponse()) {
    beepPass();
    return;
  }
  showPrescriptOnDisplay(buzzerLevelName(), 800, false, false);
}

void handleInputs() {
  ButtonEvent touchEvt = buttonPoll(touchBtn);
  ButtonEvent keyEvt = buttonPoll(keyBtn);
  ButtonEvent bootEvt = buttonPoll(bootBtn);

  if (bootEvt == BTN_TAP) cycleVolume();

  bool passTriggered = (touchEvt == BTN_TAP);
  bool failTriggered = (touchEvt == BTN_HOLD) || (keyEvt == BTN_TAP) || (keyEvt == BTN_HOLD);

  if (passTriggered || simulatePass) {
    bool fromWeb = simulatePass && !passTriggered;
    simulatePass = false;
    handlePass(fromWeb);
  }

  if (failTriggered || simulateFail || timerTimeoutFail) {
    bool fromWeb = simulateFail && !failTriggered;
    bool wasTimeout = timerTimeoutFail;
    simulateFail = false;
    timerTimeoutFail = false;
    handleFail(fromWeb, wasTimeout);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  buzzerInit();

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
  } else {
    Serial.println("LittleFS Mounted");
  }

  readSettings();
  seedDefaultPrescripts();
  buzzerLoadSettings();

  displayInit();
  displayIdle();

  bleInit();
  startAdv();

  randomSeed(esp_random());
  armAutoMission(AUTO_MISSION_MIN_MINUTES, AUTO_MISSION_MAX_MINUTES);

  buttonInit(touchBtn, PIN_TOUCH, TOUCH_ACTIVE_LEVEL, TOUCH_ACTIVE_LEVEL == HIGH ? INPUT_PULLDOWN : INPUT_PULLUP);
  buttonInit(keyBtn, PIN_KEY, LOW, INPUT_PULLUP);
  buttonInit(bootBtn, PIN_BOOT, LOW, INPUT_PULLUP);

  Serial.printf("Free heap: %u, free PSRAM: %u\n", ESP.getFreeHeap(), ESP.getFreePsram());
}

void loop() {
  handleInputs();
  handleBLE();
  handleDisplayScramble();
  handleDisplayTimer();
  handleBuzzer();
  handleSerial();
  handleAutoMission();

  if (pendingDisplayUpdate) {
    pendingDisplayUpdate = false;
    if (String(pendingDisplayText).startsWith("PIN: ")) {
      showPrescriptOnDisplay(pendingDisplayText, 30000, false, false);
    } else {
      showPrescriptOnDisplay(pendingDisplayText, 2000, false, true);
    }
  }

  if (!displayScrambling) {
    flushStatsIfNeeded();
  }

  yield();
}
