#ifndef BUZZER_H
#define BUZZER_H

#include <LittleFS.h>
#include "config.h"

#define BUZZER_MAX_STEPS 32
#define BUZZER_CHOP_HZ   1000
#define BUZZER_PWM_BITS  8
#define VOLUME_HIGH 255
#define VOLUME_MID  128
#define VOLUME_LOW  32

#define BUZZER_IDLE_LEVEL (BUZZER_ACTIVE_LEVEL == HIGH ? LOW : HIGH)

struct BeepStep {
  uint16_t onMs;
  uint16_t offMs;
};

uint8_t buzzerVolume = VOLUME_MID;
bool buzzerMuted = false;

bool textSoundsActive = false;
bool scramblePhaseSound = false;
uint16_t charClickMs = 8;
uint16_t scrambleOnMs = 8;
uint16_t scrambleOffMs = 12;
BeepStep termPattern[8] = { {40, 50}, {40, 50}, {40, 50}, {40, 80}, {200, 0} };
int termCount = 5;

static BeepStep beepSteps[BUZZER_MAX_STEPS];
static int beepCount = 0;
static int beepIndex = 0;
static bool beepOn = false;
static unsigned long beepPhaseStart = 0;
static bool stutterMode = false;

void buzzerWrite(bool on) {
  uint32_t duty = on ? buzzerVolume : 0;
  if (BUZZER_ACTIVE_LEVEL == LOW) duty = 255 - duty;
  ledcWrite(PIN_BUZZER, duty);
}

void buzzerInit() {
  ledcAttach(PIN_BUZZER, BUZZER_CHOP_HZ, BUZZER_PWM_BITS);
  buzzerWrite(false);
}

void buzzerLoadSettings() {
  File f = LittleFS.open(BUZZER_FILE, "r");
  if (!f) return;
  String content = f.readString();
  f.close();
  content.trim();
  int v = content.toInt();
  if (content == "1") v = VOLUME_HIGH;
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  buzzerMuted = (v == 0);
  if (v > 0) buzzerVolume = (uint8_t)v;
}

void buzzerStop() {
  stutterMode = false;
  beepIndex = beepCount;
  beepOn = false;
  buzzerWrite(false);
}

void saveBuzzerSettings() {
  LittleFS.remove(BUZZER_FILE);
  File f = LittleFS.open(BUZZER_FILE, "w");
  if (f) {
    f.println(String(buzzerMuted ? 0 : buzzerVolume));
    f.close();
  }
}

void setBuzzerMuted(bool muted) {
  buzzerMuted = muted;
  saveBuzzerSettings();
  if (muted) buzzerStop();
}

void setBuzzerVolume(uint8_t v) {
  if (v == 0) {
    setBuzzerMuted(true);
    return;
  }
  buzzerVolume = v;
  buzzerMuted = false;
  saveBuzzerSettings();
}

const char* buzzerLevelName() {
  if (buzzerMuted) return "Muted.";
  if (buzzerVolume >= VOLUME_HIGH) return "Volume high.";
  if (buzzerVolume >= VOLUME_MID) return "Volume mid.";
  return "Volume low.";
}

void cycleBuzzerVolume() {
  if (buzzerMuted) setBuzzerVolume(VOLUME_HIGH);
  else if (buzzerVolume >= VOLUME_HIGH) setBuzzerVolume(VOLUME_MID);
  else if (buzzerVolume >= VOLUME_MID) setBuzzerVolume(VOLUME_LOW);
  else setBuzzerMuted(true);
}

void buzzerPlay(const BeepStep* steps, int n) {
  if (buzzerMuted || n <= 0) return;
  if (n > BUZZER_MAX_STEPS) n = BUZZER_MAX_STEPS;
  stutterMode = false;
  memcpy(beepSteps, steps, n * sizeof(BeepStep));
  beepCount = n;
  beepIndex = 0;
  beepOn = true;
  beepPhaseStart = millis();
  buzzerWrite(true);
}

void buzzerPattern(int count, uint16_t onMs, uint16_t offMs) {
  BeepStep steps[BUZZER_MAX_STEPS];
  if (count > BUZZER_MAX_STEPS) count = BUZZER_MAX_STEPS;
  for (int i = 0; i < count; i++) steps[i] = { onMs, offMs };
  buzzerPlay(steps, count);
}

void buzzerStutterStart() {
  if (buzzerMuted) return;
  beepIndex = beepCount;
  stutterMode = true;
  beepOn = true;
  beepPhaseStart = millis();
  buzzerWrite(true);
}

void buzzerStutterStop() {
  if (!stutterMode) return;
  stutterMode = false;
  beepOn = false;
  buzzerWrite(false);
}

void handleBuzzer() {
  unsigned long now = millis();
  if (stutterMode) {
    if (beepOn && now - beepPhaseStart >= scrambleOnMs) {
      buzzerWrite(false);
      beepOn = false;
      beepPhaseStart = now;
    } else if (!beepOn && now - beepPhaseStart >= scrambleOffMs) {
      buzzerWrite(true);
      beepOn = true;
      beepPhaseStart = now;
    }
    return;
  }
  if (beepIndex >= beepCount) return;
  if (beepOn) {
    if (now - beepPhaseStart >= beepSteps[beepIndex].onMs) {
      buzzerWrite(false);
      beepOn = false;
      beepPhaseStart = now;
    }
  } else if (now - beepPhaseStart >= beepSteps[beepIndex].offMs) {
    beepIndex++;
    if (beepIndex < beepCount) {
      buzzerWrite(true);
      beepOn = true;
      beepPhaseStart = now;
    }
  }
}

static const BeepStep PAT_PASS[]         = { {70, 0} };
static const BeepStep PAT_FAIL[]         = { {400, 0} };
static const BeepStep PAT_CONNECTED[]    = { {50, 50}, {50, 0} };
static const BeepStep PAT_DISCONNECTED[] = { {150, 0} };
static const BeepStep PAT_TICK[]         = { {20, 0} };

#define PLAY_PATTERN(p) buzzerPlay(p, sizeof(p) / sizeof(p[0]))

void beepPass()         { PLAY_PATTERN(PAT_PASS); }
void beepFail()         { PLAY_PATTERN(PAT_FAIL); }
void beepConnected()    { PLAY_PATTERN(PAT_CONNECTED); }
void beepDisconnected() { PLAY_PATTERN(PAT_DISCONNECTED); }
void beepTick()         { PLAY_PATTERN(PAT_TICK); }

void soundTextScrambleStart() {
  if (!textSoundsActive || !scramblePhaseSound) return;
  buzzerStutterStart();
}

void soundTextScrambleEnd() {
  buzzerStutterStop();
}

void soundTextChar(char c) {
  if (!textSoundsActive || c == ' ') return;
  if (beepIndex < beepCount && beepOn) return;
  buzzerPattern(1, charClickMs, 0);
}

void soundTextDone() {
  if (!textSoundsActive) return;
  textSoundsActive = false;
  buzzerPlay(termPattern, termCount);
}

#endif
