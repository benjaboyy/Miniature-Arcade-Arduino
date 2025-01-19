//**************************************************************//
//  Name    : PatternsShiftOut                                  //
//  Author  : Benjamin Vitters                                  //
//  Version : 1.0                                               //
//  Notes   : Code for 74HC595 Shift Register with patterns     //
//**************************************************************//

// Pin connected to ST_CP of 74HC595
int latchPin = 5;
// Pin connected to SH_CP of 74HC595
int clockPin = 6;
// Pin connected to DS of 74HC595
int dataPin = 4;

// Data variables for RED and GREEN LEDs
byte dataRED;
byte dataGREEN;

// Arrays to store patterns
byte dataArrayRED[14];
byte dataArrayGREEN[14];

void setup() {
  // Set pins to output mode
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  Serial.begin(9600);

  // Define RED LED patterns (red, yellow, green repeated 4 times, then white lights)
  dataArrayRED[0] = 0x80; // Red
  dataArrayRED[1] = 0xC0; // Yellow (Red + Green)
  dataArrayRED[2] = 0x40; // Green
  dataArrayRED[3] = 0x80; // Red
  dataArrayRED[4] = 0xC0; // Yellow
  dataArrayRED[5] = 0x40; // Green
  dataArrayRED[6] = 0x80; // Red
  dataArrayRED[7] = 0xC0; // Yellow
  dataArrayRED[8] = 0x40; // Green
  dataArrayRED[9] = 0x80; // Red
  dataArrayRED[10] = 0xC0; // Yellow
  dataArrayRED[11] = 0x40; // Green
  dataArrayRED[12] = 0xFF; // White (All LEDs)
  dataArrayRED[13] = 0xFF; // White (All LEDs)

  // Define GREEN LED patterns (mirroring red for this sequence)
  dataArrayGREEN[0] = 0x00; // Red only
  dataArrayGREEN[1] = 0x40; // Yellow (Red + Green)
  dataArrayGREEN[2] = 0x80; // Green
  dataArrayGREEN[3] = 0x00; // Red only
  dataArrayGREEN[4] = 0x40; // Yellow
  dataArrayGREEN[5] = 0x80; // Green
  dataArrayGREEN[6] = 0x00; // Red only
  dataArrayGREEN[7] = 0x40; // Yellow
  dataArrayGREEN[8] = 0x80; // Green
  dataArrayGREEN[9] = 0x00; // Red only
  dataArrayGREEN[10] = 0x40; // Yellow
  dataArrayGREEN[11] = 0x80; // Green
  dataArrayGREEN[12] = 0xFF; // White (All LEDs)
  dataArrayGREEN[13] = 0xFF; // White (All LEDs)

  // Blink all LEDs to indicate setup is done
  blinkAll_2Bytes(2, 500);
}

void loop() {
  // Loop through the new pattern sequence
  for (int j = 0; j < 14; j++) {
    // Load RED and GREEN patterns
    dataRED = dataArrayRED[j];
    dataGREEN = dataArrayGREEN[j];

    // Transmit patterns
    sendToShiftRegister(dataRED, dataGREEN);
    delay(300);
  }

  // Additional patterns for more fun
  pulsePattern();
  chasePattern();
  alternatePattern();
}

// Function to send data to shift register
void sendToShiftRegister(byte redData, byte greenData) {
  digitalWrite(latchPin, LOW); // Prepare to send data
  shiftOut(dataPin, clockPin, MSBFIRST, greenData);
  shiftOut(dataPin, clockPin, MSBFIRST, redData);
  digitalWrite(latchPin, HIGH); // Latch data
}

// Shift out data (MSB first)
void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(myClockPin, LOW);
    digitalWrite(myDataPin, (myDataOut & (1 << i)) ? HIGH : LOW);
    digitalWrite(myClockPin, HIGH);
  }
}

// Blink all LEDs
void blinkAll_2Bytes(int n, int d) {
  for (int x = 0; x < n; x++) {
    sendToShiftRegister(0xFF, 0xFF);
    delay(d);
    sendToShiftRegister(0x00, 0x00);
    delay(d);
  }
}

// Cool Pattern: Pulse all LEDs
void pulsePattern() {
  for (int i = 0; i < 8; i++) {
    byte pulse = (1 << i) - 1; // Gradually light up
    sendToShiftRegister(pulse, pulse);
    delay(100);
  }
  for (int i = 7; i >= 0; i--) {
    byte pulse = (1 << i) - 1; // Gradually dim down
    sendToShiftRegister(pulse, pulse);
    delay(100);
  }
}

// Cool Pattern: Chasing LEDs
void chasePattern() {
  for (int i = 0; i < 8; i++) {
    byte chase = (1 << i); // Single light moving
    sendToShiftRegister(chase, chase);
    delay(150);
  }
}

// Cool Pattern: Alternating LEDs
void alternatePattern() {
  for (int i = 0; i < 4; i++) {
    sendToShiftRegister(0xAA, 0x55); // Alternating pattern
    delay(200);
    sendToShiftRegister(0x55, 0xAA); // Reverse alternating
    delay(200);
  }
}
