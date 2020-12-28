/*IMPORTING LIBRARIES*/

#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <FS.h>
#include <TC.h>
#include <PID_v1.h>
#include <analogWrite.h>
#include <DCE.h>

/*-------------------*/

/*DEFINE PINS*/

    // Define SPI Interface of SD Card
        #define SPISD_MOSI 17 // green
        #define SPISD_MISO 16 // yellow
        #define SPISD_SCK 5 // white
        #define SPISD_CS 4 // blue
        #define SDSPEED 4000000

    // Define SPI Interface of Thermal Couples
        #define SPITC_MISO 23 // yellow
        #define SPITC_SCK 21 // white
        #define SPITC_CS_1 19 // blue
        #define SPITC_CS_2 22 // blue

    // Define pins of controlling DC-Motors
        #define ENA 13 // green
        #define IN1 12 // yellow
        #define ENB 14
        #define IN4 27

    // Define pins of relays for PID regulation
        #define RELAY 33
  
    // Define pin of zoomer
        #define SIGNAL 2

    // Define pin of current sensor
        #define CS1 34 // White
        #define CS2 35

/*----------*/

#define HTML_OK 200

/*DEFINE GLOBAl VARIABLES*/

const char* ssid = "Extruder";
const char* password = "TerraNocturne";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

SPIClass SPISD;

TC Thermocouples[2] = {
    {SPITC_SCK, SPITC_MISO, SPITC_CS_1}, {SPITC_SCK, SPITC_MISO, SPITC_CS_2}
};

DCE_PROPERTIES engines[2] = {
    {0.0099, 0.0099, 10.2, 12, 2, 51}, {0.0099, 0.0099, 10.2, 12, 2, 51} 
};

DCE Engines[2] = {
    {engines[0], IN1, ENA, CS1}, {engines[1], IN4, ENB, CS2}
};

// Initial conditions for TC
float TC1 = 0.0, TC2 = 0.0;

// Initial conditions for PIDs
    /*
        Here must be defined new repolarization of method pid controlling
        with necessary coefficients and setpoints
    */
String pid_state = "OFF";
bool INI_RELAY_LABEL = true;
double Setpoint = 200, Input, Output;
double kp = 4, ki = 0.2, kd = 1;
PID PIDPWM(&Input, &Output, &Setpoint, kp, ki, kd, DIRECT);

String response; // Response by server

// Initial conditions for DC-Motors
float dc_pwm_1 = 100, dc_pwm_2 = 70; // Values of duty cycle
String dc_state_1 = "OFF", dc_state_2 = "OFF"; // Labels of state
/*-----------------------*/

#include <vector>
#include <iostream>

std::vector<String> path;
std::vector<String> type;
std::vector<String> url;

std::vector<String> html;
std::vector<String> css;
std::vector<String> js;


/*INITIALIZE FUNCTION BLOCK*/

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    File root = fs.open(dirname);
    if(!root){
        return;
    }
    if(!root.isDirectory()){
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            if(levels){
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            path.push_back(file.name());
        }
        file = root.openNextFile();
    }
}


void INI_SD () {

  SPISD.begin(SPISD_SCK, SPISD_MISO, SPISD_MOSI, SPISD_CS);
  SD.begin(SPISD_CS, SPISD, SDSPEED, "/sd", 25);

}

void INI_FILES () {

    listDir(SD, "/", 0);

    for (String pth: path) {
        if (pth.substring(pth.lastIndexOf(".") + 1, pth.length()) == "html") {
            if (pth == "/index.html") {
                url.push_back("/");
            } 
            else {
                url.push_back(pth);
            }            
            type.push_back("text/html");
        }
        if (pth.substring(pth.lastIndexOf(".") + 1, pth.length()) == "css") {
            url.push_back(pth);
            type.push_back("text/css");
        }
        if (pth.substring(pth.lastIndexOf(".") + 1, pth.length()) == "js") {
            url.push_back(pth);
            type.push_back("text/javascript");
        }
    }

}

void INI_PER () {

    pinMode(SIGNAL, ANALOG);
    pinMode(RELAY, ANALOG);

    analogWrite(SIGNAL, 0, 255);
    analogWrite(RELAY, 0, 255);

}

void INI_PID () {

    Input = Thermocouples[0].T;
    PIDPWM.SetMode(AUTOMATIC);

}


void ZOOMER () {

    analogWrite(SIGNAL, 150, 255);
    delay(100);
    analogWrite(SIGNAL, 0, 255);
    delay(100);
    analogWrite(SIGNAL, 150, 255);
    delay(100);
    analogWrite(SIGNAL, 0, 255);

}

String PATH;
String TYPE;
String URL;

void function (AsyncWebServerRequest *request, int i) {
    request->send(SD, path[i], type[i]);
}

void INI_WB () {

    WiFi.softAP(ssid, password);

    delay(100);

    for (int i = 0; i < path.size(); i++) {
        PATH = path[i];
        TYPE = type[i];
        URL = url[i];
        server.on(URL, HTTP_GET,  [=] (AsyncWebServerRequest *request) {
            function(request, i);
        });

    }
    server.begin();

}

void READ_TEMTERATURES () {

    unsigned long time_delay = 300;
    
    TC1 = Thermocouples[0].get_temperature();

    unsigned long time_start = millis();
    unsigned long time_current = millis();
    while(time_current - time_start <= time_delay){
        time_current = millis();
    }  
    
    TC2 = Thermocouples[1].get_temperature();

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
    analogWrite(RELAY, Output, 255);

}

void setup() {

    Serial.begin(9600);
    INI_SD();
    INI_FILES();


    //INI_PER();
    //INI_PID();    
    //INI_WB();

}

void loop() {

    READ_TEMTERATURES();
    if (pid_state == "ON") {REGULARATION();};   

}