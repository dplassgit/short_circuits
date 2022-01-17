/**
  Turns on, then off, each output pin of the motherboard, in sequence.

  This looks like what was on the chip when I received it originally.
*/

// Which pin we just turned on
int pin;
// Maximum pin number
#define MAX 20

void setup() {
  for (int i = 2; i < MAX; ++i) {
    pinMode(i, OUTPUT);
  }
  pin = 2;
}

void loop() {
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);
  digitalWrite(pin, LOW);
  pin++;
  if (pin == MAX) pin = 2;
}
