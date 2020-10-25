#include <TC.h>

TC::TC(int8_t sck, int8_t miso, int8_t cs) {

  SCK = sck;
  CS = cs;
  MISO = miso;

  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(MISO, INPUT);

  digitalWrite(CS, HIGH);

}

float TC::get_temperature() {

  uint16_t temp;

  digitalWrite(CS, LOW);
  delayMicroseconds(10);

  temp = spi_transfer();
  temp <<= 8;
  temp |= spi_transfer();

  digitalWrite(CS, HIGH);

  if (temp & 0x4) {

    return NAN;

  }

  temp >>= 3;

  T = temp * 0.25;

  return T;
  
}

byte TC::spi_transfer() {

  byte temp = 0;

  for (int i = 7; i >= 0; i--) {

    digitalWrite(SCK, LOW);
    delayMicroseconds(10);

    if (digitalRead(MISO)) {

      temp |= (1 << i);

    }

    digitalWrite(SCK, HIGH);
    delayMicroseconds(10);

  }

  return temp;

}