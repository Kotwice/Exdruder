/*SCHEMA*/
/*

                          SHIFT REGISTER 74HC595
                                 ________
   SPI_MISO (SPICS TC2) <-  Q1 -|1 \_/16|- Vcc  -> 3.3V
                            Q2 -|2    15|- Q0   -> SPI_MISO (SPICS TC1)
                            Q3 -|3    14|- DS   -> SPISR_MOSI (ESP32)
                            Q4 -|4    13|- -OE  -> GND
                            Q5 -|5    12|- STCP -> SPISR_CS (ESP32)
                            Q6 -|6    11|- SHCP -> SPISR_SCK (ESP32)
                            Q7 -|7    10|- -MR  -> 3.3V
                    GND <- GND -|8_____9|- Q7S


*/
/*------*/

// LOLA IS HERE!
//
// Kotwice
// kot
// pseudo
// lol//
/*IMPORT LIBRARIES*/
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <FS.h>
#include <PID_v1.h>
#include <SparkFunSX1509.h>
#include <ShiftRegister.h>

/*PID*/
double Setpoint = 200, Input, Output;
double kp = 4, ki = 0.2, kd = 1;
PID PIDPWM(&Input, &Output, &Setpoint, kp, ki, kd, DIRECT);
/*---*/

/*DEFINE PINS*/
#define SPISD_MOSI 17
#define SPISD_MISO 16
#define SPISD_SCK 5
#define SPISD_CS 4
#define SDSPEED 4000000

#define SPISR_CS 19
#define SPISR_MISO 23
#define SPISR_SCK 21
#define SPISR_MOSI 18

#define I2C_SDA 0
#define I2C_SCL 2

#define SX1509_ADDRESS 0x3E
#define SX1509_SPEAKER 0
#define SX1509_RELAY_PID 2
#define SX1509_ENA 3
#define SX1509_IN1 4
#define SX1509_ENB 5
#define SX1509_IN4 6
#define SX1509_SIGNAL 7

#define HTML_OK 200

#define DAC 25
/*----------*/

/*DEFINE GLOBAl VARIABLES*/
const char* ssid = "Extruder";
const char* password = "TerraNocturne";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

SPIClass SPISD;

SX1509 IO;

float TC1 = 0.0, TC2 = 0.0;
String pid_state = "OFF";
bool INI_RELAY_LABEL = true;
String response;

float dc_pwm_1 = 100, dc_pwm_2 = 70;
String dc_state_1 = "OFF", dc_state_2 = "OFF";

String audio_state = "OFF";

ShiftRegister SR;
/*-----------------------*/

/*INITIALIZE BLOCK*/
void INI_SD () {

  SPISD.begin(SPISD_SCK, SPISD_MISO, SPISD_MOSI, SPISD_CS);
  SD.begin(SPISD_CS, SPISD, SDSPEED, "/sd", 25);

}

void INI_SR () {

    SR.begin(SPISR_SCK, SPISR_MISO, SPISR_MOSI, SPISR_CS);

}

void INI_I2C () {

    Wire.begin(I2C_SDA, I2C_SCL);

}

void INI_SX1509 () {

    IO.begin(SX1509_ADDRESS);   
    
    IO.pinMode(SX1509_IN1, OUTPUT);
    IO.pinMode(SX1509_IN4, OUTPUT);

    IO.pinMode(SX1509_SPEAKER, ANALOG_OUTPUT);
    IO.pinMode(SX1509_RELAY_PID, ANALOG_OUTPUT);
    IO.pinMode(SX1509_ENA, ANALOG_OUTPUT);
    IO.pinMode(SX1509_ENB, ANALOG_OUTPUT);
    IO.pinMode(SX1509_SIGNAL, ANALOG_OUTPUT);
     
    IO.digitalWrite(SX1509_IN1, LOW);  
    IO.digitalWrite(SX1509_IN4, LOW);  

    IO.analogWrite(SX1509_SPEAKER, 255); 
    IO.analogWrite(SX1509_RELAY_PID, 255);
    IO.analogWrite(SX1509_ENA, 255);
    IO.analogWrite(SX1509_ENB, 255);
    IO.analogWrite(SX1509_SIGNAL, 255);

}

void INI_PID () {

    Input = TC1;
    PIDPWM.SetMode(AUTOMATIC);

}

void SIGNAL () {

    IO.analogWrite(SX1509_SIGNAL, 150);
    delay(100);
    IO.analogWrite(SX1509_SIGNAL, 255);
    delay(100);
    IO.analogWrite(SX1509_SIGNAL, 150);
    delay(100);
    IO.analogWrite(SX1509_SIGNAL, 255);

}

void initiate_sd () {

  SPISD.begin(SPISD_SCK, SPISD_MISO, SPISD_MOSI, SPISD_CS);
  SD.begin(SPISD_CS, SPISD, SDSPEED, "/sd", 25);

}

void INI_WB () {

    WiFi.softAP(ssid, password);

    delay(100);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/index.html", String(), false);
    });  

    server.on("/graph.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/graph.html", "text/html");
    });

    server.on("/pid.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/pid.html", "text/html");
    }); 

    server.on("/dc.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/dc.html", "text/html");
    }); 

    server.on("/prog_1.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/prog_1.html", "text/html");
    }); 

    server.on("/audio.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/audio.html", "text/html");
    }); 

    server.on("/background.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/background.css", "text/css");
    });

    server.on("/content.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/content.css", "text/css");
    });

    server.on("/menu.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/menu.css", "text/css");
    });

    server.on("/pid.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/pid.css", "text/css");
    });

    server.on("/slider.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/slider.css", "text/css");
    });

     server.on("/switch.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/switch.css", "text/css");
    });

    server.on("/graph.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/graph.js", "text/javascript");
    });

    server.on("/pid.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/pid.js", "text/javascript");
    });

    server.on("/dc.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/dc.js", "text/javascript");
    });
    
    server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/main.js", "text/javascript");
    });

    server.on("/audio.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/audio.js", "text/javascript");
    });
    
    server.on("/plotly-latest.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SD, "/plotly-latest.min.js", "text/javascript");
    });    

    server.on("/jquery-3.4.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SD, "/jquery-3.4.1.min.js", "text/javascript");
    });

    server.on("/temperatures", HTTP_GET, [](AsyncWebServerRequest *request) {
        response =  "[{'value': " + String(TC1) + "}, {'value':" + String(TC2) + "}]";
        request->send(HTML_OK, "text/plain", response);
    });

    server.on("/pid", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ki")) {
            ki = request->getParam("ki")->value().toFloat();
            kp = request->getParam("kp")->value().toFloat();
            kd = request->getParam("kd")->value().toFloat();
            Setpoint = request->getParam("setpoint")->value().toFloat();        
        }
        if (request->hasParam("pid_state")) {
            pid_state = request->getParam("pid_state")->value();   
            if (pid_state == "ON") {
                IO.analogWrite(SX1509_RELAY_PID, 255 - Output);
                SIGNAL();   
            }
            else {
                IO.analogWrite(SX1509_RELAY_PID, 255); 
                SIGNAL();                  
            }             
        }
        response = "[{'kp': " + String(kp) + ", 'ki': " + String(ki) + ", 'kd': " + 
            String(kd) + ",'setpoint': " + String(Setpoint) + ",'pid_state':" + "'" + pid_state + "'" + "}]";
        Serial.println(response);
        request->send(HTML_OK, "text/plain");
    });

    server.on("/pid_ini", HTTP_GET, [](AsyncWebServerRequest *request) {
        response = "[{'kp': " + String(kp) + ", 'ki': " + String(ki) + ", 'kd': " + 
            String(kd) + ",'setpoint': " + String(Setpoint) + " ,'pid_state': " + "'" + pid_state + "'" + "}]";
        request->send(HTML_OK, "text/plain", response);
    });
    
    server.on("/dc", HTTP_GET, [](AsyncWebServerRequest *request) {

        if (request->hasParam("dc_pwm_1")) {
            dc_pwm_1 = request->getParam("dc_pwm_1")->value().toInt();
            IO.analogWrite(SX1509_ENA, dc_pwm_1);
        }

        if (request->hasParam("dc_pwm_2")) {
            dc_pwm_2 = request->getParam("dc_pwm_2")->value().toInt();
            IO.analogWrite(SX1509_ENB, dc_pwm_2);
        }

        if (request->hasParam("dc_state_1")) {
            dc_state_1 = request->getParam("dc_state_1")->value();
            if (dc_state_1 == "ON") {
                IO.digitalWrite(SX1509_IN1, HIGH);
                SIGNAL();
            }
            else {
                IO.digitalWrite(SX1509_IN1, LOW);
                SIGNAL();
            }
        }

        if (request->hasParam("dc_state_2")) {
            dc_state_2 = request->getParam("dc_state_2")->value();
            if (dc_state_2 == "ON") {
                IO.digitalWrite(SX1509_IN4, HIGH);
                SIGNAL();
            }
            else {
                IO.digitalWrite(SX1509_IN4, LOW);
                SIGNAL();
            }
        }

        response = "[{'dc_pwm': " + String(dc_pwm_1) + ", 'dc_state': " + "'" + dc_state_1 + "'" + 
            "}, {'dc_pwm': " + String(dc_pwm_2) + ", 'dc_state': " + "'" + dc_state_2 + "'" + "}]"; 

        Serial.println(response);

        request->send(HTML_OK, "text/plain");
    });

    server.on("/dc_ini", HTTP_GET, [](AsyncWebServerRequest *request) {
        response = "[{'dc_pwm': " + String(dc_pwm_1) + ", 'dc_state': " + "'" + dc_state_1 + "'" + 
            "}, {'dc_pwm': " + String(dc_pwm_2) + ", 'dc_state': " + "'" + dc_state_2 + "'" + "}]";         
        request->send(HTML_OK, "text/plain", response);
    });

    server.begin();

}

void READ_TEMTERATURES () {

    int ADRESS_TC1[] = {0, 0, 0, 0, 0, 0, 1, 0};
    int ADRESS_TC2[] = {0, 0, 0, 0, 0, 0, 0, 1};

    unsigned long time_delay = 300;
    
    TC1 = SR.read(ADRESS_TC1);

    unsigned long time_start = millis();
    unsigned long time_current = millis();
    while(time_current - time_start <= time_delay){
        time_current = millis();
    }  
    
    TC2 = SR.read(ADRESS_TC2);

    time_start = millis();
    time_current = millis();
    while(time_current - time_start <= time_delay){
        time_current = millis();
    }

}

void REGULARATION () {

    Input = TC1;
    PIDPWM.SetTunings(kp, ki, kd);
    PIDPWM.Compute();    
    IO.analogWrite(SX1509_RELAY_PID, 255 - Output);

}

void setup() {

    Serial.begin(9600);
    INI_SD();
    INI_SR();
    INI_I2C();
    INI_SX1509();
    INI_PID();    
    INI_WB();

}

void loop() {

    READ_TEMTERATURES();
    if (pid_state == "ON") {REGULARATION();};   

}