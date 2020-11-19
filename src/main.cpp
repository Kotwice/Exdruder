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

/*INITIALIZE FUNCTION BLOCK*/

/*
void INI_FILES (fs::FS &fs, const char * directory, uint8_t levels) {

    int legnth = 25;

    String* path = new String[legnth];
    String* type = new String[legnth];

    File root = fs.open(directory);

    if(!root){
        return;
    }

    if(!root.isDirectory()){
        return;
    }

    File file = root.openNextFile();

    int i = 0;

    while(file){

        if(file.isDirectory()){

            if(levels){

                file.listDir(fs, file.name(), levels - 1);

            }

        } else {

            path[i] = file.name();
            type[i] = path[i].indexOf(".")

            Serial.print(" FILE: ");
            Serial.print(file.name());
            Serial.print(" SIZE: ");
            Serial.println(file.size());

            file.name()

        }

        file = root.openNextFile();

        i++;

    }

    int LENGTH = 20;
    
    const String* PATH = new String[LENGTH];
    String* TYPE = new String[LENGTH];
    String* URL = new String[LENGTH];

    const String& path = "a";
    const String& type = "A";
    
    for (int i = 0; i < LENGTH; i++) {    
        server.on("/", HTTP_GET, [path, type] (AsyncWebServerRequest *request) {
            request->send(SD, path, type);
        });
    }


}

    // listDir(SD, "/", 0);
*/

void INI_SD () {

  SPISD.begin(SPISD_SCK, SPISD_MISO, SPISD_MOSI, SPISD_CS);
  SD.begin(SPISD_CS, SPISD, SDSPEED, "/sd", 25);

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
                analogWrite(RELAY, Output, 255);
                ZOOMER();   
            }
            else {
                analogWrite(RELAY, 0, 255); 
                ZOOMER();                  
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
            //analogWrite(ENA, dc_pwm_1, 255);
            Engines[0].set_pwm(dc_pwm_1);
            Serial.println(Engines[0].get_current());
        }

        if (request->hasParam("dc_pwm_2")) {
            dc_pwm_2 = request->getParam("dc_pwm_2")->value().toInt();
            analogWrite(ENB, dc_pwm_2, 255);
        }

        if (request->hasParam("dc_state_1")) {
            dc_state_1 = request->getParam("dc_state_1")->value();
            if (dc_state_1 == "ON") {
                digitalWrite(IN1, HIGH);
                ZOOMER();
            }
            else {
                digitalWrite(IN1, LOW);
                analogWrite(ENA, 0, 255);
                ZOOMER();
            }
        }

        if (request->hasParam("dc_state_2")) {
            dc_state_2 = request->getParam("dc_state_2")->value();
            if (dc_state_2 == "ON") {
                digitalWrite(IN4, HIGH);
                ZOOMER();
            }
            else {
                digitalWrite(IN4, LOW);
                analogWrite(ENB, 0, 255);
                ZOOMER();
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
    INI_PER();
    INI_PID();    
    INI_WB();

}

void loop() {

    READ_TEMTERATURES();
    if (pid_state == "ON") {REGULARATION();};   

}