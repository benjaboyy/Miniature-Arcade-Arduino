#define GRAY 0x8410

// todo:
// - Fuse with other code
// - Make scroll speed adjustable in serial
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "ST7789_AVR.h"
#include "bitmap.h"  // Ensure this contains a 320x170 image in RGB565 format
#include "scoreboard.h"  // Ensure this contains a 320x170 image in RGB565 format

#define COLR1 0x2104
#define COLR2 0xF8B2  // Lighter pink
#define COLR3 0xF520

#define TFT_DC   10
#define TFT_CS   8
#define TFT_RST  7

#define LOGICAL_W  170  // Logical width after rotation (physical height)
#define LOGICAL_H  320  // Logical height after rotation (physical width)

// Define the band in which the notes will scroll (rotated 90°)
// Here we use the full screen width (170) and a fixed height (150)
// Then we center that band vertically.
#define NOTES_AREA_W LOGICAL_W
#define NOTES_AREA_H 135
#define NOTES_AREA_X 10
#define NOTES_AREA_Y 10

// Custom lane configuration
#define LANE_COUNT 8
const int laneWidths[LANE_COUNT] = {30, 15, 15, 15, 15, 15, 15, 15};
const uint16_t laneColors[LANE_COUNT] = {RED, WHITE, BLUE, WHITE, BLUE, WHITE, BLUE, WHITE};
int laneX[LANE_COUNT];

// Calculate the X position of each lane (left edge)
#define NOTE_SPEED 9    // Pixels per loop iteration
#define NOTE_THICKNESS 6  // Thickness of the note (after rotation)

ST7789_AVR lcd = ST7789_AVR(TFT_DC, TFT_RST, TFT_CS);

struct Note {
  int x;  // In rotated mode, x becomes the scrolling coordinate.
  int y;  // y remains fixed per lane.
  bool active;
};

int combo = 0;  // Track the combo count
int maxCombo = 0;  // Track the highest combo achieved
int counter = 0;
int counterInterval = 0;

const int MAX_NOTES = 20;
Note notes[MAX_NOTES];


void spawnNote(int lane) {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (!notes[i].active) {
      // For scrolling from right to left, start at the right edge of the notes area, y is lane top
      notes[i] = { NOTES_AREA_X + NOTES_AREA_W - NOTE_THICKNESS, laneX[lane], true };
      break;
    }
  }
}

void updateNotes() {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (notes[i].active) {
      // Find which lane this note is in
      int lane = -1;
      for (int l = 0; l < LANE_COUNT; l++) {
        if (notes[i].y == laneX[l]) {
          lane = l;
          break;
        }
      }
      int laneH = laneWidths[lane];
      // Erase the previous note drawing.
      lcd.fillRect(notes[i].x, notes[i].y + 1, NOTE_THICKNESS, laneH - 4, BLACK);

      // Move the note leftwards.
      notes[i].x -= NOTE_SPEED;

      // If it moves beyond the left edge of the notes area, deactivate it.
      if (notes[i].x < NOTES_AREA_X) {
        notes[i].active = false;
      } else {
        lcd.fillRect(notes[i].x, notes[i].y + 1, NOTE_THICKNESS, laneH - 4, laneColors[lane]);
      }
    }
  }
}

// Groovy animation colors
const uint16_t groovyColors[6] = {RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
int groovyFrame = 0;
unsigned long lastGroovyUpdate = 0;

void drawGroovySquare() {
  // Position: right of the lanes
  int x = NOTES_AREA_X + NOTES_AREA_W - 25; // 10px gap from lanes
  int y = NOTES_AREA_Y + 30; // 30px from top
  int size = 75;
  // Animate: fill with cycling color
  uint16_t color = groovyColors[groovyFrame % 6];
  lcd.fillRect(x, y, size, size, color);
  // Draw more animated lines for extra movement
  for (int i = 0; i < 5; i++) {
    int hoff = (groovyFrame * (3 + i) + i * 17) % size;
    int voff = (groovyFrame * (4 + i) + i * 23) % size;
    lcd.drawFastHLine(x, y + hoff, size, groovyColors[(groovyFrame + i) % 6]);
    lcd.drawFastVLine(x + voff, y, size, groovyColors[(groovyFrame + 5 - i) % 6]);
  }
}

void drawLanes() {
  // Clear only the notes area (so we don’t erase the entire background).
  lcd.fillRect(NOTES_AREA_X, NOTES_AREA_Y, NOTES_AREA_W, NOTES_AREA_H, BLACK);

  // Draw horizontal lane dividers at each lane boundary (rotated layout)
  for (int i = 0; i <= LANE_COUNT; i++) {
    int y = (i == 0) ? NOTES_AREA_Y : laneX[i-1];
  lcd.drawFastHLine(NOTES_AREA_X, y, NOTES_AREA_W, GRAY);
  }
}

void drawCombo() {
  static unsigned long lastFlashTime = 0;
  static bool flashState = false;
  unsigned long now = millis();
  if (now - lastFlashTime >= 500) {
    flashState = !flashState;
    lastFlashTime = now;
  }
  uint16_t flashColor = flashState ? WHITE : 0x7D7C; // 0x7D7C is a light blue (RGB565)
  lcd.setTextColor(flashColor, COLR1);  // Flashing text color with background
  lcd.setTextSize(2);

  lcd.setCursor(35, 60);
  lcd.print("Great:");
  if (combo < 10) {
    lcd.print("0");  // Print leading zero for numbers < 10
  }
  lcd.print(combo);
}

void drawCounter() {
  
  lcd.setTextColor(WHITE, COLR1);  // White text with black background to clear previous text
  lcd.setTextSize(1);

  lcd.setCursor(252, 113);
  if (counter < 10) {
    lcd.print("0");  // Print leading zero for numbers < 10
  }
  if (counter < 100) {
    lcd.print("0");  // Print leading zero for numbers < 10
  }
  lcd.print(counter);
  lcd.print("0");
}

void noteHit() {
  combo++;  // Increase combo
  if (combo > maxCombo) maxCombo = combo;  // Track max combo
  drawCombo();  // Update the screen
}

void noteMissed() {
  combo = 0;  // Reset the combo
  drawCombo();  // Update the screen
}

void checkSerial() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'd') {  // Check for the character directly
      Serial.println("Correct key! Restarting setup...");
      delay(100);  // Give time for serial print before resetting
      asm volatile ("jmp 0");  // Reset the Arduino (alternative method)
    }
  }
}

void setup() {
  
  // Calculate laneX positions
  laneX[0] = NOTES_AREA_X;
  for (int i = 1; i < LANE_COUNT; i++) {
    laneX[i] = laneX[i-1] + laneWidths[i-1];
  }
  Serial.begin(9600);
  lcd.init(LOGICAL_W, LOGICAL_H);
  lcd.fillScreen(RED);
  
  counter = 0;
  lcd.setRotation(1);  // Rotate the entire screen
  lcd.setTextColor(WHITE);
  lcd.setTextSize(3);
  lcd.setCursor((LOGICAL_H - (6 * 8 * 2)) / 2, (LOGICAL_W - (8 * 2)) / 2);
  lcd.print("KONMAI");
  delay(4000);
  lcd.setRotation(0);  // Reset back to normal rotation
  
  lcd.fillScreen(COLR1);
  // Draw the background bitmap (this remains unrotated).
  lcd.drawBitmap(0, 0, epd_bitmap_Layer_3, LOGICAL_W, LOGICAL_H, COLR2);
  lcd.drawBitmap(0, 0, epd_bitmap_Layer_4, LOGICAL_W, LOGICAL_H, COLR3);
  
  // Draw the initial lanes in the notes area.
  drawLanes();
}

void loop() {
  checkSerial();
  // Randomly spawn a note in one of the 5 lanes.
  if (random(10) < 2) {
    spawnNote(random(LANE_COUNT));
  }
  updateNotes();
  lcd.setRotation(1);  // Rotate the entire screen
  // Only trigger noteHit() once in 20 iterations
  if (random(20) == 0) {  
    noteHit();
  }
  // Occasionally break the combo (e.g., 1 in 50 chance)
  if (random(50) == 0) {  
    noteMissed();
  }
  // Always update the combo display for flashing effect
  drawCombo();
  // Draw groovy animation square (slower update)
  static unsigned long lastGroovyUpdate = 0;
  unsigned long now = millis();
  if (now - lastGroovyUpdate > 700) { // slower animation
    groovyFrame++;
    lastGroovyUpdate = now;
  }
  drawGroovySquare();
  counter++;
  counterInterval++;
  if (counterInterval >= 5) {
    drawCounter();
    counterInterval = 0;
  }
  if (counter >= 300) {
    setup();
  }
  lcd.setRotation(0);  // Rotate the entire screen
  delay(100);
}
