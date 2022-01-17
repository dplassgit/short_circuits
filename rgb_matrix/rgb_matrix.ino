/**
   Turns on LED in the matrix, in each color.
*/

const int SER = 3;
const int CLK = 4;
const int LATCH = 5;
const int OE = 10;

void setup() {
  // Start Serial Comminication
  Serial.begin(9600);

  // DIGITISER Stuff
  pinMode(SER, OUTPUT); // SER (data) pin as output
  pinMode(CLK, OUTPUT); // CLK (clock) pin as output
  pinMode(LATCH, OUTPUT); // LATCH pin as output
  pinMode(OE, OUTPUT); // Output Enable

  analogWrite(OE, 0); // Set brightness to full 
}

// lower 4 bits of data1: active low row select, xxxxabcd a=top d=bottom
// upper 4 bits of data1: active HIGH red color per column 1234
// data0 has color of the column in pairs: bbbbgggg (12341234) active high

// 1. row (active low) in data1 xxxxrrrr
// 2. color (active high) for that column: red in data1 rrrrxxxx, blue and green in data0 bbbbgggg
void turnOn(int row, int column, int red, int green, int blue) {
  int data1 = 0;
  switch (row) {
    // i am so lazy
    case 0:
      data1 = 0b00001110;
      break;
    case 1:
      data1 = 0b00001101;
      break;
    case 2:
      data1 = 0b00001011;
      break;
    case 3:
      data1 = 0b00000111;
      break;
  }
  int data0 = 0;
  switch (column) {
    case 0:
      data1 |= 0b00010000 * red;
      data0 |= 0b00010000 * blue;
      data0 |= 0b00000001 * green;
      break;
    case 1:
      data1 |= 0b00100000 * red;
      data0 |= 0b00100000 * blue;
      data0 |= 0b00000010 * green;
      break;
    case 2:
      data1 |= 0b01000000 * red;
      data0 |= 0b01000000 * blue;
      data0 |= 0b00000100 * green;
      break;
    case 3:
      data1 |= 0b10000000 * red;
      data0 |= 0b10000000 * blue;
      data0 |= 0b00001000 * green;
      break;
  }
  digitalWrite(LATCH, LOW);
  shiftOut(SER, CLK, LSBFIRST, data0);
  shiftOut(SER, CLK, LSBFIRST, data1);
  digitalWrite(LATCH, HIGH);
}

int row = 0;
int column = 0;
int color = 0b001; // bgr

void loop() {
  // oneLedOnLoop();
  allLedsOnLoop();
}

int iteration = 0;
/**
  Turns on all the LEDS on using the color in "color".
  Leaves them on for 200 "iterations" (about 1 ms per iteration).
  Then goes to the next "color".
*/
void allLedsOnLoop() {
  for (row = 0; row < 4; ++row) {
    for (column = 0; column < 4; ++column) {
      turnOn(row, column, color & 1, (color & 2) >> 1, (color & 4) >> 2);
    }
  }
  iteration++;
  if (iteration == 200) {
    color++;
    if (color == 8) {
      color = 1;
    }
    iteration = 0;
  }
}

/**
  Turn the LED at "row", "column" to the current "color", then goes to the next column,
  then the next row, then the next color.
*/
void oneLedOnLoop() {
  turnOn(row, column, color & 1, (color & 2) >> 1, (color & 4) >> 2);
  column++;
  if (column == 4) {
    column = 0;
    row++;
  }
  if (row == 4) {
    row = 0;
    color++;
    if (color == 8) {
      color = 1;
    }
    //    Serial.print("Color is now: ");
    //    Serial.println(color, BIN);
  }
  delay(200);
}
