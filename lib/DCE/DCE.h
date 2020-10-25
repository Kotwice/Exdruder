#include <Arduino.h>
#include <ACS712.h>
#include <analogWrite.h>

struct DCE_PROPERTIES {
    float K_E, K_T, R, U_0, I_n, Z;
};

class DCE {

    public:
        DCE(DCE_PROPERTIES engine, int8_t in, int8_t en, int8_t cs);
        void stop();
        void set_rpm(float n);
        void set_voltage(float u);
        float get_current();
        void set_pwm(uint16_t pwm);
        float U = 0, I = 0, N = 0, M = 0;
        bool ERROR = false;
        bool SUCCESS = false;
        uint16_t PWM = 0;

    private:
        float K_E, K_T, R, U_0, I_n, Z;
        int8_t IN, EN, CS; // IN - logical pin of LN298, EN - pmw pin of LN298; cs - analog pin of current sensor;
        uint16_t ADC_RES = 1023; // resolution of analog to digital convecter, set 10 bit;
        int PWM_FREQ = 1000; // set 1kH frequency;
        ACS712* SENSOR;
        void iteration(int i, float n);

};