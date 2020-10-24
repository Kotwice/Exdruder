#include <TC.h>

TC::TC(int8_t SCK, int8_t MISO, int8_t CS) {

  sck = SCK;
  cs = CS;
  miso = MISO;

  pinMode(cs, OUTPUT);
  pinMode(sck, OUTPUT);
  pinMode(miso, INPUT);

  digitalWrite(cs, HIGH);

}

float TC::read(void) {

  uint16_t v;

  digitalWrite(cs, LOW);
  delayMicroseconds(10);

  v = spiread();
  v <<= 8;
  v |= spiread();

  digitalWrite(cs, HIGH);

  if (v & 0x4) {

    return NAN;

  }

  v >>= 3;

  return v * 0.25;
  
}

byte TC::spiread(void) {

  int i;
  byte d = 0;

  for (i = 7; i >= 0; i--) {
    digitalWrite(sck, LOW);
    delayMicroseconds(10);
    if (digitalRead(miso)) {
      // set the bit to 0 no matter what
      d |= (1 << i);
    }

    digitalWrite(sck, HIGH);
    delayMicroseconds(10);
  }

  return d;

}