#include <Arduino.h>

class ShiftRegister {
    private:

        int8_t _SCK,  _MISO, _MOSI, _CS;
        void select(int ADRESS[]);
    public:        
        void begin(int8_t SCK, int8_t MISO, int8_t MOSI, int8_t CS);        
        float read(int ADRESS[]);
};

void ShiftRegister::begin(int8_t SCK, int8_t MISO, int8_t MOSI, int8_t CS) {

    _SCK = SCK;
    _MISO = MISO;
    _MOSI = MOSI;
    _CS = CS;

    pinMode(_SCK, OUTPUT);
    pinMode(_MISO, INPUT);
    pinMode(_MOSI, OUTPUT);
    pinMode(_CS, OUTPUT);
    digitalWrite(_CS, HIGH);

}

void ShiftRegister::select(int ADRESS[]) {

    digitalWrite(_CS, LOW);

    delayMicroseconds(1000);

    for (int i = 0; i < 8; i++) {

        digitalWrite(_SCK, LOW);
        delayMicroseconds(1000);

        if (ADRESS[i] == 0) {
            digitalWrite(_MOSI, LOW);
        }
        else {
            digitalWrite(_MOSI, HIGH);  
        }

        digitalWrite(_SCK, HIGH);
        delayMicroseconds(1000);

    }

    digitalWrite(_CS, HIGH);

}

float ShiftRegister::read(int ADRESS[]) {

    select(ADRESS);

    uint16_t value;
    byte temp = 0;

    for (int i = 7; i >= 0; i--) {

        digitalWrite(_SCK, LOW);

        delayMicroseconds(1000);

        if (digitalRead(_MISO)) {
        
            temp |= (1 << i);

        }

        digitalWrite(_SCK, HIGH);

        delayMicroseconds(1000);

    }

    value = temp;

    value <<= 8;

    temp = 0;

    for (int i = 7; i >= 0; i--) {

        digitalWrite(_SCK, LOW);

        delayMicroseconds(1000);

        if (digitalRead(_MISO)) {
        
            temp |= (1 << i);

        }

        digitalWrite(_SCK, HIGH);

        delayMicroseconds(1000);

    }

    value |= temp;    

    if (value & 0x4) {    
        return NAN;        
    }

    value >>= 3;

    return 0.25 * value;

}
