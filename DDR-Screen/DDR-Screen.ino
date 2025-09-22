// todo:
// - Fuse with other code
// - Make scroll speed adjustable in serial

#include <Arduino.h>
#include <U8g2lib.h>

// I²C version of GME64128-02 (likely ST7567 controller)
// If it doesn't work, try the ST7565 constructor below instead
U8G2_ST7567_JLX12864_F_HW_I2C lcd(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// Alternative (uncomment if your module is ST7565):
// U8G2_ST7565_NHD_C12864_F_HW_I2C lcd(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define SCR_WD 128
#define SCR_HT 64

// Notes area: centered vertically, 40px tall
#define NOTES_AREA_W SCR_WD
#define NOTES_AREA_H 40
#define NOTES_AREA_X 0
#define NOTES_AREA_Y ((SCR_HT - NOTES_AREA_H) / 2)

#define LANE_COUNT 5
#define LANE_HEIGHT (NOTES_AREA_H / LANE_COUNT)
int noteSpeed = 2;          // Adjustable via serial
#define NOTE_THICKNESS 3    // Slim note bars

struct Note {
  int x;
  int y;
  bool active;
};

const int MAX_NOTES = 20;
Note notes[MAX_NOTES];

int combo = 0;
int maxCombo = 0;
int counter = 0;
int counterInterval = 0;

// -------------------- Notes --------------------
void spawnNote(int lane) {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (!notes[i].active) {
      notes[i] = { NOTES_AREA_X + NOTES_AREA_W - NOTE_THICKNESS, 
                   NOTES_AREA_Y + lane * LANE_HEIGHT, 
                   true };
      break;
    }
  }
}

void updateNotes() {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (notes[i].active) {
      notes[i].x -= noteSpeed;
      if (notes[i].x < NOTES_AREA_X) {
        notes[i].active = false;
      }
    }
  }
}

// -------------------- Drawing --------------------
void drawLanes() {
  lcd.drawFrame(NOTES_AREA_X, NOTES_AREA_Y, NOTES_AREA_W, NOTES_AREA_H);
  for (int i = 1; i < LANE_COUNT; i++) {
    lcd.drawHLine(NOTES_AREA_X, NOTES_AREA_Y + i * LANE_HEIGHT, NOTES_AREA_W);
  }
}

void drawNotes() {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (notes[i].active) {
      lcd.drawBox(notes[i].x, notes[i].y + 1, NOTE_THICKNESS, LANE_HEIGHT - 2);
    }
  }
}

void drawCombo() {
  lcd.setFont(u8g2_font_6x10_tf);
  lcd.setCursor(2, 10);
  lcd.print("HIT:");
  lcd.print(combo);
}

void drawCounter() {
  lcd.setFont(u8g2_font_6x10_tf);
  lcd.setCursor(SCR_WD - 40, 10);
  lcd.print("CNT:");
  lcd.print(counter);
}

// -------------------- Game Logic --------------------
void noteHit() {
  combo++;
  if (combo > maxCombo) maxCombo = combo;
}

void noteMissed() {
  combo = 0;
}

void checkSerial() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '+') { noteSpeed++; }
    if (c == '-') { if (noteSpeed > 1) noteSpeed--; }
    if (c == 'd') { asm volatile ("jmp 0"); }
  }
}

// -------------------- Setup & Loop --------------------
void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.clearBuffer();

  // Splash screen
  lcd.setFont(u8g2_font_6x10_tf);
  lcd.drawStr((SCR_WD - 6 * 6) / 2, SCR_HT / 2, "KONMAI");
  lcd.sendBuffer();
  delay(2000);
}

void loop() {
  checkSerial();

  if (random(10) < 2) {
    spawnNote(random(LANE_COUNT));
  }

  updateNotes();

  // Simulated hit/miss
  if (random(20) == 0) noteHit();
  if (random(50) == 0) noteMissed();

  counter++;
  counterInterval++;

  // -------------------- Drawing --------------------
  lcd.clearBuffer();
  drawLanes();
  drawNotes();
  drawCombo();
  if (counterInterval >= 5) {
    drawCounter();
    counterInterval = 0;
  }
  lcd.sendBuffer();

  if (counter >= 300) setup();

  delay(100);
}
