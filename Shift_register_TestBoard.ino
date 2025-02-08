/**************************************************************
 *  Name    : CoolLightsPatternLinked
 *  Author  : Benjamin Vitters
 *  Version : 0.5
 *  Description: A lights pattern sketch for your Arduino 
 *               miniature arcade using a 74HC595 shift register.
 *               Chip 1 patterns are stored in chip1Pattern.
 *               Chip 2 patterns are stored in chip2.
 *               Several animations that play with the LED colors
 *               are implemented below.
 **************************************************************/

//----------------------
// Pin Definitions
//----------------------
const int latchPin = 5;   // Pin connected to ST_CP of 74HC595
const int clockPin = 6;   // Pin connected to SH_CP of 74HC595
const int dataPin  = 4;   // Pin connected to DS of 74HC595

//----------------------
// Global Variables & Arrays
//----------------------
byte chip1Data;           // Data for Chip 1 (red LED)
byte chip2Data;           // Data for Chip 2 (green LED)

// Predefined linked patterns (14 steps)
byte chip1Pattern[14];    // Chip 1 controls red LED patterns.
byte chip2[14];           // Chip 2 controls green LED patterns.

//----------------------
// Setup
//----------------------
void setup() {
  // Set pins as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Initialize Serial (optional for debugging)
  Serial.begin(9600);
  
  // (Optional) Seed the random generator
  randomSeed(analogRead(0));

  // Define linked LED patterns for Chip 1
  chip1Pattern[0]  = 0b10000000; // Red (Bit 7)
  chip1Pattern[1]  = 0b01000000; // Yellow (Bit 6)
  chip1Pattern[2]  = 0b00100000; // Green (Bit 5)
  chip1Pattern[3]  = 0b00010000; // Red (Bit 4)
  chip1Pattern[4]  = 0b00001000; // Yellow (Bit 3)
  chip1Pattern[5]  = 0b00000100; // Green (Bit 2)
  chip1Pattern[6]  = 0b00000010; // Red (Bit 1)
  chip1Pattern[7]  = 0b00000001; // Yellow (Bit 0)

  // Define linked LED patterns for Chip 2
  chip2[0]  = 0b00000001; // Green
  chip2[1]  = 0b00000010; // Red
  chip2[2]  = 0b00000100; // Yellow
  chip2[3]  = 0b00001000; // Green
  chip2[4]  = 0b00010000; // White
  chip2[5]  = 0b00100000; // White
  chip2[6]  = 0b01000000; // White
  chip2[7]  = 0b10000000; // White

  // Blink all LEDs to indicate that setup is complete
  blinkAll(2, 500);
}

//----------------------
// Main Loop: Run Animations in Sequence
//----------------------
void loop() {
  runAnimations();
}

//----------------------
// Animation Runner: Calls each animation in sequence.
//----------------------
void runAnimations() {
  animateRed();
  animateGreen();
  animateYellow();
  animateWhite();
  animateCycle();           // Cycle through the linked pattern sequence
  animateSwirl();           // Red moves one way, green the opposite
  animatePulse();           // Pulse (brightness effect) using both colors
  animateRandomTwinkle();   // Random patterns on both chips
  animateColorFade();       // Fade from red to green and back
}

//----------------------
// Animation Functions
//----------------------


// Animate “All Red” using the red-designated LED positions.
void animateRed() {
  byte redChip1 = chip1Pattern[1] | chip1Pattern[4] | chip1Pattern[6];
  byte redChip2 = chip2[1]; // Adjust indices as per your setup
  sendToShiftRegister(redChip1, redChip2);
  delay(300);
}

// Animate “All Green” using the green-designated LED positions.
void animateGreen() {
  byte greenChip1 = chip1Pattern[2] | chip1Pattern[5];
  byte greenChip2 = chip2[0] | chip2[3];
  sendToShiftRegister(greenChip1, greenChip2);
  delay(300);
}

// Animate “All Yellow” using the yellow-designated LED positions.
void animateYellow() {
  byte yellowChip1 = chip1Pattern[0] | chip1Pattern[3] | chip1Pattern[7];
  byte yellowChip2 = chip2[2];
  sendToShiftRegister(yellowChip1, yellowChip2);
  delay(300);
}

// Animate “All white using the white-designated LED positions.
void animateWhite() {
  byte whiteChip1;
  byte whiteChip2 = chip2[4] | chip2[5] | chip2[6] | chip2[7];
  sendToShiftRegister(whiteChip1, whiteChip2);
  delay(300);
}

void animateCycle() {
  // Cycle forward (from bit 0 to bit 7)
  for (int pos = 0; pos < 8; pos++) {
    byte pattern = 1 << pos;  // Shifts a '1' into the desired position.
    sendToShiftRegister(pattern, pattern);
    delay(300);
  }
  // Cycle backward (from bit 6 back to bit 0)
  for (int pos = 6; pos >= 0; pos--) {
    byte pattern = 1 << pos;
    sendToShiftRegister(pattern, pattern);
    delay(300);
  }
}

// Swirl Animation: Red (Chip 1) rotates one way while Green (Chip 2) rotates oppositely.
void animateSwirl() {
  byte chip1pattern   = 0b10000000;  // Start with leftmost LED for red
  byte chip2pattern = 0b00000001;  // Start with rightmost LED for green
  for (int i = 0; i < 8; i++) {
    sendToShiftRegister(chip1pattern, chip2pattern);
    delay(150);
    chip1pattern   = (chip1pattern >> 1) | (chip1pattern << 7);     // Rotate right
    chip2pattern = (chip2pattern << 1) | (chip2pattern >> 7);   // Rotate left
  }
}

// Pulse Animation: Simulate brightness pulse by increasing then decreasing the number of lit LEDs.
void animatePulse() {
  // Increase brightness
  for (int i = 1; i <= 8; i++) {
    byte pattern = 0xFF << (8 - i); // e.g., i=1: 0x80, i=2: 0xC0, etc.
    sendToShiftRegister(pattern, pattern);
    delay(100);
  }
  // Decrease brightness
  for (int i = 7; i > 0; i--) {
    byte pattern = 0xFF << (8 - i);
    sendToShiftRegister(pattern, pattern);
    delay(100);
  }
}

// Random Twinkle Animation: Display random patterns on both chips.
void animateRandomTwinkle() {
  for (int i = 0; i < 20; i++) {
    chip1Data = random(0, 256);
    chip2Data = random(0, 256);
    sendToShiftRegister(chip1Data, chip2Data);
    delay(100);
  }
}

// Color Fade Animation: Gradually shift from chip1 to chip2 and back using brightness levels.
void animateColorFade() {
  const byte brightnessLevels[9] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
  // Fade from chip1 (full) to green (full)
  for (int i = 0; i < 9; i++) {
    byte chip1Value   = brightnessLevels[8 - i];  // chip1 decreases
    byte chip2Value = brightnessLevels[i];        // chip2 increases
    sendToShiftRegister(chip1Value, chip2Value);
    delay(150);
  }
  // Fade back from chip1 to chip1
  for (int i = 0; i < 9; i++) {
    byte chip1Value   = brightnessLevels[i];
    byte chip2Value = brightnessLevels[8 - i];
    sendToShiftRegister(chip1Value, chip2Value);
    delay(150);
  }
}

//----------------------
// Utility Functions
//----------------------

// Sends two bytes (one for each chip) to the 74HC595 shift register.
void sendToShiftRegister(byte chip1Data, byte chip2Data) {
  digitalWrite(latchPin, LOW);                     // Prepare for data transmission
  shiftOut(dataPin, clockPin, MSBFIRST, chip2Data);  // Send Chip 2 data
  shiftOut(dataPin, clockPin, MSBFIRST, chip1Data);    // Send Chip 1 data
  digitalWrite(latchPin, HIGH);                    // Latch the data to output
}

// Blink all LEDs on and off, 'n' times with delay 'd' milliseconds.
void blinkAll(int n, int d) {
  for (int i = 0; i < n; i++) {
    sendToShiftRegister(0xFF, 0xFF);
    delay(d);
    sendToShiftRegister(0x00, 0x00);
    delay(d);
  }
}
