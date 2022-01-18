/**
   Demos: Turns on LED in the matrix, in each color.
*/

const int SER = 3;
const int CLK = 4;
const int LATCH = 5;
const int OE = 10;

void setup() {
  // Start Serial Comminication
  Serial.begin(9600);

  // RGB Matrix Stuff
  pinMode(SER, OUTPUT); // SER (data) pin as output
  pinMode(CLK, OUTPUT); // CLK (clock) pin as output
  pinMode(LATCH, OUTPUT); // LATCH pin as output
  pinMode(OE, OUTPUT); // Output Enable

  analogWrite(OE, 50); // Set brightness to medium
}

// lower 4 bits of data1: active low row select, xxxxabcd a=top d=bottom
// upper 4 bits of data1: active HIGH red color per column 1234
// data0 has color of the column in pairs: bbbbgggg (12341234) active high

// 1. row (active low) in data1 xxxxrrrr
// 2. color (active high) for each column: red in data1 rrrrxxxx, blue and green in data0 bbbbgggg
void turnOn(int row, int column, int rgb) {
  int data1 = 0;
  // row is active low, so have to ~ it, but colors are active high so have to & it.
  data1 = (~(1 << row)) & 0b00001111;
  int data0 = 0;
  if (rgb & 0b001) {
    // red
    data1 |= 0b00010000 << column;
  }
  if (rgb & 0b010) {
    // green
    data0 |= 1 << column;
  }
  if (rgb & 0b100) {
    // blue
    data0 |= 0b00010000 << column;
  }

  digitalWrite(LATCH, LOW);
  shiftOut(SER, CLK, LSBFIRST, data0);
  shiftOut(SER, CLK, LSBFIRST, data1);
  digitalWrite(LATCH, HIGH);
}

int mode = 0;
void loop() {
  if (mode) {
    oneLedOnLoop();
  } else {
    allLedsOnLoop();
  }
}

int color = 0b001; // bgr
int iteration = 0;
/**
  Turns on all the LEDS on using the color in "color".
  Leaves them on for 200 "iterations" (about 1 ms per iteration).
  Then goes to the next "color".
*/
void allLedsOnLoop() {
  for (int row = 0; row < 4; ++row) {
    for (int column = 0; column < 4; ++column) {
      turnOn(row, column, color);
    }
  }
  iteration++;
  if (iteration == 200) {
    color++;
    if (color == 8) {
      mode = 1 - mode;
      color = 1;
    }
    iteration = 0;
  }
}

int row = 0;
int column = 0;

/**
  Turn the LED at "row", "column" to the current "color", then goes to the next column,
  then the next row, then the next color.
*/
void oneLedOnLoop() {
  turnOn(row, column, color);
  column++;
  if (column == 4) {
    column = 0;
    row++;
  }
  if (row == 4) {
    row = 0;
    color++;
    if (color == 8) {
      mode = 1 - mode;
      color = 1;
    }
    Serial.print("Color is now: ");
    Serial.println(color, BIN);
  }
  delay(100);
}
