#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

extern int textScale;
extern int scrambleDurationFrames;
extern int scrambleDelayMs;
extern int revealDelayMs;
extern int timerPosition;
extern float timerScale;
extern bool bleRequirePin;
extern bool useProportionalFont;
extern uint16_t textColor;
extern bool timerFormatLong;

#define TFT_WIDTH   320
#define TFT_HEIGHT  170
#define TFT_ROTATION 3
#define SCREEN_INSET_TOP    5
#define SCREEN_INSET_BOTTOM 0
#define SCREEN_INSET_LEFT   0
#define SCREEN_INSET_RIGHT  0
#define DRAW_WIDTH  (TFT_WIDTH - SCREEN_INSET_LEFT - SCREEN_INSET_RIGHT)
#define DRAW_HEIGHT (TFT_HEIGHT - SCREEN_INSET_TOP - SCREEN_INSET_BOTTOM)

#define PIN_LCD_BL   38
#define PIN_LCD_D0   39
#define PIN_LCD_D1   40
#define PIN_LCD_D2   41
#define PIN_LCD_D3   42
#define PIN_LCD_D4   45
#define PIN_LCD_D5   46
#define PIN_LCD_D6   47
#define PIN_LCD_D7   48
#define PIN_POWER_ON 15
#define PIN_LCD_RES  5
#define PIN_LCD_CS   6
#define PIN_LCD_DC   7
#define PIN_LCD_WR   8
#define PIN_LCD_RD   9

#define PIN_TOUCH    16
#define PIN_BUZZER   10
#define PIN_KEY      14
#define PIN_BOOT     0
#define PIN_BAT_VOLT 4

#define TOUCH_ACTIVE_LEVEL   HIGH
#define BUZZER_ACTIVE_LEVEL  HIGH

#define DEBOUNCE_DELAY   50
#define HOLD_DELAY_MS    700

#define COUNTDOWN_TICK_SECONDS 3

#define LINE_BUF   256
#define MAX_LINES  8
#define WORD_BUF   64

#define AUTO_MISSION_MIN_MINUTES 10
#define AUTO_MISSION_MAX_MINUTES 30

#define BLE_CHUNK_SIZE      20
#define BLE_CHUNK_DELAY_MS  10

#define CUSTOMS_FILE    "/customs.txt"
#define STATS_FILE      "/stats.txt"
#define SETTINGS_FILE   "/settings.txt"
#define BUZZER_FILE     "/buzzer.txt"
#define SEEDED_FILE     "/seeded.txt"

#define DEFAULT_TEXT_SCALE          2
#define DEFAULT_SCRAMBLE_FRAMES     20
#define DEFAULT_SCRAMBLE_DELAY      20
#define DEFAULT_REVEAL_DELAY        20

#define COLOR_BG    0x0000
#define DEFAULT_TEXT_COLOR 0xBF5F

inline bool isSystemText(const char* t) {
  return strcmp(t, "CLEAR.") == 0 || strcmp(t, "FAILED.") == 0 ||
         strcmp(t, "Connected.") == 0 || strcmp(t, "Disconnected.") == 0 ||
         strcmp(t, "NO DATA") == 0 || strcmp(t, "Muted.") == 0 ||
         strncmp(t, "Volume", 6) == 0 || strncmp(t, "PIN: ", 5) == 0;
}

#endif
