/**
   Text: Write letters and numbers.
*/
#include "font.h"

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

  analogWrite(OE, 0); // Set brightness to full
}

char saved = 0;
void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    char typed = Serial.read();
    if (typed != 10) {
      saved = typed;
    }
  }
  if (saved != 0) {
    writeChar(saved);
  }
}

void writeString(const char *string) {
  for (int i = 0; i < strlen(string); ++i) {
    writeChar(string[i]);
  }
}

void writeChar(char c) {
  c -= ' '; // offset from space
  int base = (c / 2) * 4; // integer division
//  Serial.println("base");
//  Serial.println(base);
  const uint8_t *letter = TINYFONT + base;
  for (int i = 0; i < 4; ++i) {
    uint8_t colData = pgm_read_byte(letter++);
    if ((c % 2) == 0) {
      // mask upper sprite
      colData &= 0x0f;
    } else {
      // shift down
      colData >>= 4;
    }
    // now, draw this column
    for (int col = 0; col < 4; ++col) {
      if (colData & 1) {
        // yes it's backwards.
        turnOn(col, i, 7);
      }
      colData >>= 1;
    }
  }
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
