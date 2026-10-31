#ifndef BUTTONS_H
#define BUTTONS_H

#include "config.h"

enum ButtonEvent { BTN_NONE, BTN_TAP, BTN_HOLD };

struct Button {
  uint8_t pin;
  uint8_t activeLevel;
  int lastRaw;
  bool pressed;
  bool holdFired;
  unsigned long lastChange;
  unsigned long pressStart;
};

void buttonInit(Button& b, uint8_t pin, uint8_t activeLevel, uint8_t mode) {
  pinMode(pin, mode);
  b.pin = pin;
  b.activeLevel = activeLevel;
  b.lastRaw = digitalRead(pin);
  b.pressed = (b.lastRaw == activeLevel);
  b.holdFired = true;
  b.lastChange = millis();
  b.pressStart = b.lastChange;
}

ButtonEvent buttonPoll(Button& b) {
  int raw = digitalRead(b.pin);
  unsigned long now = millis();
  if (raw != b.lastRaw) {
    b.lastRaw = raw;
    b.lastChange = now;
  }
  if (now - b.lastChange < DEBOUNCE_DELAY) return BTN_NONE;

  bool down = (raw == b.activeLevel);
  if (down && !b.pressed) {
    b.pressed = true;
    b.holdFired = false;
    b.pressStart = now;
    return BTN_NONE;
  }
  if (down && !b.holdFired && now - b.pressStart >= HOLD_DELAY_MS) {
    b.holdFired = true;
    return BTN_HOLD;
  }
  if (!down && b.pressed) {
    b.pressed = false;
    return b.holdFired ? BTN_NONE : BTN_TAP;
  }
  return BTN_NONE;
}

#endif
