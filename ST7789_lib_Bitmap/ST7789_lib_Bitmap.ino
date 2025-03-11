#include <SPI.h>
#include <Adafruit_GFX.h>
#include "ST7789_AVR.h"
#include "bitmap.h"  // Ensure this contains a 320x170 image in RGB565 format
#include "scoreboard.h"  // Ensure this contains a 320x170 image in RGB565 format

#define TFT_DC   10
#define TFT_CS   8
#define TFT_RST  7

#define SCR_WD  170
#define SCR_HT  320

// Define the band in which the notes will scroll (rotated 90°)
// Here we use the full screen width (170) and a fixed height (150)
// Then we center that band vertically.
#define NOTES_AREA_W SCR_WD
#define NOTES_AREA_H 150
#define NOTES_AREA_X 0
#define NOTES_AREA_Y ((SCR_HT - NOTES_AREA_H) / 2)

#define LANE_COUNT 5
#define LANE_HEIGHT (NOTES_AREA_H / LANE_COUNT)
#define NOTE_SPEED 4    // Pixels per loop iteration
#define NOTE_THICKNESS 5  // Thickness of the note (after rotation)

ST7789_AVR lcd = ST7789_AVR(TFT_DC, TFT_RST, TFT_CS);

struct Note {
  int x;  // In rotated mode, x becomes the scrolling coordinate.
  int y;  // y remains fixed per lane.
  bool active;
};

const int MAX_NOTES = 20;
Note notes[MAX_NOTES];

// Define a different color for each lane.
uint16_t laneColors[LANE_COUNT] = { RED, GREEN, BLUE, YELLOW, MAGENTA };

void spawnNote(int lane) {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (!notes[i].active) {
      // For scrolling from right to left, start at the right edge of the notes area.
      notes[i] = { NOTES_AREA_X + NOTES_AREA_W - NOTE_THICKNESS, NOTES_AREA_Y + lane * LANE_HEIGHT, true };
      break;
    }
  }
}

void updateNotes() {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (notes[i].active) {
      // Erase the previous note drawing.
      lcd.fillRect(notes[i].x, notes[i].y + 1, NOTE_THICKNESS, LANE_HEIGHT - 4, BLACK);
      
      // Move the note leftwards.
      notes[i].x -= NOTE_SPEED;
      
      // If it moves beyond the left edge of the notes area, deactivate it.
      if (notes[i].x < NOTES_AREA_X) {
        notes[i].active = false;
      } else {
        // Determine the lane based on the note's y coordinate.
        int lane = (notes[i].y - NOTES_AREA_Y) / LANE_HEIGHT;
        lcd.fillRect(notes[i].x, notes[i].y + 1, NOTE_THICKNESS, LANE_HEIGHT - 4, laneColors[lane]);
      }
    }
  }
}

void drawLanes() {
  // Clear only the notes area (so we don’t erase the entire background).
  lcd.fillRect(NOTES_AREA_X, NOTES_AREA_Y, NOTES_AREA_W, NOTES_AREA_H, BLACK);
  
  // Draw horizontal lane dividers.
  for (int i = 1; i < LANE_COUNT; i++) {
    lcd.drawFastHLine(NOTES_AREA_X, NOTES_AREA_Y + i * LANE_HEIGHT, NOTES_AREA_W, WHITE);
  }
  // Optionally, draw vertical borders for the notes area.
  lcd.drawFastVLine(NOTES_AREA_X, NOTES_AREA_Y, NOTES_AREA_H, WHITE);
  lcd.drawFastVLine(NOTES_AREA_X + NOTES_AREA_W - 1, NOTES_AREA_Y, NOTES_AREA_H, WHITE);
}

void setup() {
  Serial.begin(9600);
  lcd.init(SCR_WD, SCR_HT);
  lcd.setRotation(0);  // Leave this unchanged so the background remains as drawn.
  
  lcd.fillScreen(BLUE);
  // Draw the background bitmap (this remains unrotated).
  // lcd.drawBitmap(0, 0, epd_bitmap_BG__2_, SCR_WD, SCR_HT, RED);
  lcd.drawBitmap(0, 0, epd_bitmap_Layer_6, SCR_WD, SCR_HT, GREEN);
  
  // Draw the initial lanes in the notes area.
  drawLanes();
}

void loop() {
  // Randomly spawn a note in one of the 5 lanes.
  if (random(10) < 2) {
    spawnNote(random(LANE_COUNT));
  }
  updateNotes();
  delay(100);
}
