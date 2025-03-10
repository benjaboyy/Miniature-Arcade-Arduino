#include <SPI.h>
#include <Adafruit_GFX.h>
#include "ST7789_AVR.h"

#define TFT_DC   10
#define TFT_CS   8 // Without CS
#define TFT_RST  7 // Without CS

#define SCR_WD  170
#define SCR_HT  320

ST7789_AVR lcd = ST7789_AVR(TFT_DC, TFT_RST, TFT_CS);

#include "bitmap.h"  // Ensure this contains a 320x170 image in RGB565 format
#include "scoreboard.h"  // Ensure this contains a 320x170 image in RGB565 format

void setup() {
  Serial.begin(9600);
  lcd.init(SCR_WD, SCR_HT); // Initialize with correct dimensions
  lcd.setRotation(0);        // Adjust if necessary
  lcd.fillScreen(BLACK);

  // Display the 320x170 image at (0,0)
  lcd.drawBitmap(0, 0, epd_bitmap_BG__2_, SCR_WD, SCR_HT, RED);
  lcd.drawBitmap(0, 1, epd_bitmap_BG__2_, SCR_WD, SCR_HT, GREEN);
  lcd.drawBitmap(0, 2, epd_bitmap_BG__2_, SCR_WD, SCR_HT, BLUE);
  lcd.drawBitmap(0, 3, epd_bitmap_BG__2_, SCR_WD, SCR_HT, WHITE);
}

void loop() {
  // No animation, just display the image
}