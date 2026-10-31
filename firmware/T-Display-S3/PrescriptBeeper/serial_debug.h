#ifndef SERIAL_DEBUG_H
#define SERIAL_DEBUG_H

#include "config.h"
#include "buzzer.h"

void showPrescriptOnDisplay(const char* text, unsigned long durationMs, bool infinite, bool respond);
void drawNewPrescript();
void armAutoMission(unsigned long minutesMin, unsigned long minutesMax);
extern unsigned long nextAutoMissionAt;
extern String currentDisplayText;

static String serialLine = "";

int parseSteps(const String& spec, BeepStep* steps, int maxSteps) {
  int n = 0;
  int pos = 0;
  while (pos < (int)spec.length() && n < maxSteps) {
    int c1 = spec.indexOf(',', pos);
    String onStr = spec.substring(pos, c1 == -1 ? spec.length() : c1);
    if (c1 == -1) {
      steps[n++] = { (uint16_t)onStr.toInt(), 0 };
      break;
    }
    int c2 = spec.indexOf(',', c1 + 1);
    String offStr = spec.substring(c1 + 1, c2 == -1 ? spec.length() : c2);
    steps[n++] = { (uint16_t)onStr.toInt(), (uint16_t)offStr.toInt() };
    if (c2 == -1) break;
    pos = c2 + 1;
  }
  return n;
}

void handleSerialCommand(String line) {
  line.trim();
  if (line.length() == 0) return;

  if (line.startsWith("BEEP ")) {
    BeepStep steps[BUZZER_MAX_STEPS];
    int n = parseSteps(line.substring(5), steps, BUZZER_MAX_STEPS);
    buzzerPlay(steps, n);
    Serial.println("OK BEEP");
  } else if (line.startsWith("PAT ")) {
    String name = line.substring(4);
    if (name == "pass") beepPass();
    else if (name == "fail") beepFail();
    else if (name == "connected") beepConnected();
    else if (name == "disconnected") beepDisconnected();
    else if (name == "tick") beepTick();
    else if (name == "term") buzzerPlay(termPattern, termCount);
    else { Serial.println("ERR unknown pattern"); return; }
    Serial.println("OK PAT " + name);
  } else if (line.startsWith("SHOW ")) {
    showPrescriptOnDisplay(line.substring(5).c_str(), 3000, false, false);
    Serial.println("OK SHOW");
  } else if (line.startsWith("CLICK ")) {
    charClickMs = (uint16_t)line.substring(6).toInt();
    Serial.println("OK CLICK " + String(charClickMs));
  } else if (line.startsWith("SCR ")) {
    String v = line.substring(4);
    int c = v.indexOf(',');
    if (c == -1) {
      scramblePhaseSound = (v.toInt() != 0);
    } else {
      scramblePhaseSound = true;
      scrambleOnMs = (uint16_t)v.substring(0, c).toInt();
      scrambleOffMs = (uint16_t)v.substring(c + 1).toInt();
    }
    Serial.println("OK SCR " + String(scramblePhaseSound) + " " + String(scrambleOnMs) + "," + String(scrambleOffMs));
  } else if (line.startsWith("TERM ")) {
    termCount = parseSteps(line.substring(5), termPattern, 8);
    Serial.println("OK TERM " + String(termCount));
  } else if (line.startsWith("MUTE ")) {
    setBuzzerMuted(line.substring(5).toInt() != 0);
    Serial.println(buzzerMuted ? "OK MUTED" : "OK UNMUTED");
  } else if (line == "NEXT") {
    drawNewPrescript();
    Serial.println("OK NEXT " + currentDisplayText);
  } else if (line.startsWith("AUTO ")) {
    unsigned long m = line.substring(5).toInt();
    armAutoMission(m, m);
    Serial.println("OK AUTO " + String(m) + " min");
  } else if (line == "AUTO") {
    long remain = nextAutoMissionAt == 0 ? -1 : (long)(nextAutoMissionAt - millis()) / 1000;
    Serial.println("OK AUTO in " + String(remain) + " s");
  } else if (line.startsWith("VOL ")) {
    int v = line.substring(4).toInt();
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    setBuzzerVolume((uint8_t)v);
    Serial.println(String("OK VOL ") + buzzerLevelName() + " (" + String(v) + ")");
  } else if (line == "SEED") {
    writeDefaultPrescripts();
    Serial.println("OK SEED " + String(countStoredPrescripts()) + " prescripts");
  } else if (line == "BAT") {
    uint32_t mv = analogReadMilliVolts(PIN_BAT_VOLT) * 2;
    Serial.println("OK BAT " + String(mv) + " mV");
  } else if (line == "STOP") {
    buzzerStop();
    Serial.println("OK STOP");
  } else if (line == "HELP") {
    Serial.println("SHOW text | BEEP on,off,... | PAT pass|fail|connected|disconnected|tick|term | CLICK ms | SCR 0|1|on,off | TERM on,off,... | MUTE 0|1 | NEXT | AUTO [minutes] | VOL 0-255 | SEED | BAT | STOP");
  } else {
    Serial.println("ERR unknown command, try HELP");
  }
}

void handleSerial() {
  int budget = 128;
  while (Serial.available() && budget-- > 0) {
    int r = Serial.read();
    if (r < 0) break;
    char c = (char)r;
    if (c == '\n' || c == '\r') {
      handleSerialCommand(serialLine);
      serialLine = "";
    } else if (serialLine.length() < 200) {
      serialLine += c;
    }
  }
}

#endif
