/**************************************************************
 *  Name    : Miniature-Arcade-Arduino (Single Shift Register)
 *  Author  : Benjamin Vitters
 *  Version : 0.7
 *  Description: A lights pattern sketch for your Arduino 
 *               miniature arcade using a single 74HC595 shift register.
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
byte chipData;            // Data for the shift register

bool idleOff = true;

unsigned long lastCommandTime = 0;
const unsigned long idleTimeout = 5000;

// Predefined LED patterns (example uses 8 steps for simplicity)
byte chipPattern[8] = {
  0b01000000, // Red (Bit 7)
  0b10000000, // Yellow (Bit 6)
  0b00100000, // Green (Bit 5)
  0b00010000, // Red (Bit 4)
  0b00001000, // Yellow (Bit 3)
  0b00000100, // Green (Bit 2)
  0b00000010, // Red (Bit 1)
  0b00000001  // Yellow (Bit 0)
};

//----------------------
// Setup
//----------------------
void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(0));
  animatePulse();
  blank();
}

//----------------------
// Main Loop
//----------------------
void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    processCommand(command);
    lastCommandTime = millis();
  }
  if (millis() - lastCommandTime > idleTimeout && idleOff) {
    animateIdle();
    lastCommandTime = millis();
  }
}

//----------------------
// Command Processing Function
//----------------------
void processCommand(char command) {
  switch (command) {
    case 'r': animateRed(); Serial.println("Red"); break;
    case 'g': animateGreen(); Serial.println("Green"); break;
    case 'y': animateYellow(); Serial.println("Yellow"); break;
    case 'p': animatePulse(); Serial.println("Pulse"); break;
    case 'c': animateCycle(); Serial.println("Cycle"); break;
    case 'b': blank(); Serial.println("Blank"); break;
    default: Serial.print("Unknown: "); Serial.println(command); break;
  }
}

//----------------------
// Animation Functions
//----------------------
void animateRed() {
  sendToShiftRegister(chipPattern[0] | chipPattern[3] | chipPattern[6]);
  delay(500);
}

void animateYellow() {
  sendToShiftRegister(chipPattern[1] | chipPattern[4] | chipPattern[7]);
  delay(500);
}

void animateGreen() {
  sendToShiftRegister(chipPattern[2] | chipPattern[5]);
  delay(500);
}

void animatePulse() {
  for (int i = 1; i <= 8; i++) {
    sendToShiftRegister(0xFF << (8 - i));
    delay(100);
  }
  for (int i = 7; i > 0; i--) {
    sendToShiftRegister(0xFF << (8 - i));
    delay(100);
  }
}

void animateCycle() {
  for (int pos = 0; pos < 8; pos++) {
    sendToShiftRegister(1 << pos);
    delay(200);
  }
  for (int pos = 6; pos >= 0; pos--) {
    sendToShiftRegister(1 << pos);
    delay(200);
  }
}

void animateIdle() {
  void (*animations[])() = {animateCycle, animatePulse};
  animations[random(0, 2)]();
  blank();
}

void blank() {
  sendToShiftRegister(0x00);
  delay(300);
}

//----------------------
// Utility Functions
//----------------------
void sendToShiftRegister(byte chipData) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, chipData);
  digitalWrite(latchPin, HIGH);
}
