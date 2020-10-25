#include <DCE.h>

DCE::DCE(DCE_PROPERTIES engine, int8_t in, int8_t en, int8_t cs) {

    K_E = engine.K_E;
    K_T = engine.K_T;
    R = engine.R;
    U_0 = engine.U_0;
    I_n = engine.I_n;
    Z = engine.Z;
    IN = in;
    EN = en;
    CS = cs;

    pinMode(IN, OUTPUT);
    pinMode(EN, ANALOG);
   
    analogWriteFrequency(EN, PWM_FREQ);

    digitalWrite(IN, LOW);
    analogWrite(EN, PWM, ADC_RES);

    SENSOR = new ACS712(ACS712_05B, CS);

}

void DCE::stop() {

    digitalWrite(IN, LOW);
    analogWrite(EN, 0, ADC_RES);

}

float DCE::get_current() {

    if (digitalRead(IN) == HIGH){
        return SENSOR->getCurrentDC();
    } else {
        return 0.0;
    }    

}

void DCE::set_voltage(float u) {

    U = u;
    PWM = (int) U / U_0 * ADC_RES;
    analogWrite(EN, PWM, ADC_RES);
    
}

void DCE::iteration(int i, float n) {

    float epsilon = 0.1;

    I = get_current();

    if (I > I_n) {
        ERROR = true;
        stop();
        return;
    }

    analogWrite(EN, i, ADC_RES);
    U = i / ADC_RES * U_0;
    N = U / K_E - K_T * R * I;

    if (abs(N - n * Z) < epsilon) {
        PWM = i;
        M = K_T * I;
        SUCCESS = true;
        return;
    }

}

void DCE::set_rpm(float n) {

    if(digitalRead(IN) == LOW) {

        digitalWrite(IN, HIGH);

    }  

    if (n * Z > N) {

        for (int i = PWM; i < ADC_RES + 1; i++) {

            if (ERROR | SUCCESS) {
                break;
            }

            iteration(i, n);

        } 

    } else {

        for (int i = PWM; i >= 0; i--) {

            if (ERROR | SUCCESS) {
                break;
            }

            iteration(i, n);

        } 
    }

}

void DCE::set_pwm(uint16_t pwm) {

    PWM = pwm;
    analogWrite(EN, PWM, ADC_RES);

}