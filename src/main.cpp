#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <time.h>
#include "Boiler.h"
#include "../.pio/libdeps/esp32dev/ServoESP32/src/Servo.h"
#include <HTTPClient.h>
#include <Arduino.h>
#include "../.pio/libdeps/esp32dev/ArduinoJson/ArduinoJson.h"

typedef struct tm tm;

tm* getTime(){
    time_t now = time(nullptr);
    return localtime(&now);
}

char ssid[] = "Brennholzverleih";
char pass[] = "Einhornaufzuchtstation";
int status;

Servo servo1;
const int pushButtonUp = 32;
const int pushButtonDown = 25;
const int LED = 33;

Boiler boiler0(&getTime, &servo1);

void printWifiData() {
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println(ip);

    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    Serial.print(mac[5], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.println(mac[0], HEX);

}


void wifiConnect(){
    
    while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
    }


    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network:
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(5000);
        if(status == WL_CONNECT_FAILED) Serial.println("Connection failed");
        if(status == WL_NO_SSID_AVAIL) Serial.println("No SSID available");
        if(status == WL_NO_SHIELD) Serial.println("WiFi shield not present");
    }
    digitalWrite(LED, HIGH);
    // you're connected now, so print out the data:
    Serial.print("You're connected to the network");
    printWifiData();
    
}

void printTime(tm * time){
    Serial.printf("%d:%d:%d\n", time->tm_hour, time->tm_min, time->tm_sec);
}

void updateTime(){
    if(getTime()->tm_hour == 3 && getTime()->tm_min == 0){
        configTime(3600, 3600, "pool.ntp.org");
    }
}

void printSettings(DynamicJsonDocument data){
    
    Serial.println(data["hoursDay"].as<String>());
    Serial.println(data["minutesDay"].as<String>());
    Serial.println(data["hoursNight"].as<String>());
    Serial.println(data["minutesNight"].as<String>());
    Serial.println(data["stateNight"].as<String>());
    Serial.println(data["stateDay"].as<String>());
    
}

void getSettings(){
    HTTPClient http;
    
    http.begin("http://192.168.178.63/set?get");

    int httpResponseCode = http.GET();
    
    if(httpResponseCode == 200){
        DynamicJsonDocument tmp(1024);
        deserializeJson(tmp, http.getString());
        boiler0.setTimeDay(tmp["hoursDay"].as<int>(), tmp["minutesDay"].as<int>());
        boiler0.setTimeNight(tmp["hoursNight"].as<int>(), tmp["minutesNight"].as<int>());
        boiler0.setStateDay(boiler0.itostate((tmp["stateDay"].as<int>())));
        boiler0.setStateNight(boiler0.itostate((tmp["stateNight"].as<int>())));
        boiler0.update();
        Serial.printf("Time Day: %d:%d State: %d\n", tmp["hoursDay"].as<int>(), tmp["minutesDay"].as<int>(), tmp["stateDay"].as<int>());
        Serial.printf("Time Night: %d:%d State: %d\n", tmp["hoursNight"].as<int>(), tmp["minutesNight"].as<int>(), tmp["stateNight"].as<int>());

    }
}

int setSettings(){
    HTTPClient http;
    
    DynamicJsonDocument data(1024);
    data["hoursDay"] = String(boiler0.getHoursDay());
    data["minutesDay"] = String(boiler0.getMinutesDay());
    data["hoursNight"] = String(boiler0.getHoursNight());
    data["minutesNight"] = String(boiler0.getMinutesNight());
    data["stateNight"] = String(boiler0.statetoi(boiler0.getStateNight()));
    data["stateDay"] = String(boiler0.statetoi(boiler0.getStateDay()));
    
    http.begin("http://192.168.178.63/settings");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(data.as<String>());
    
    return httpResponseCode;
};

void timedSleep(){
    
    int sleep_night = (boiler0.getHoursNight() * 60) + boiler0.getMinutesNight() - (getTime()->tm_hour * 60 + getTime()->tm_min);
    int sleep_day = (boiler0.getHoursDay() * 60) + boiler0.getMinutesDay() - (getTime()->tm_hour * 60 + getTime()->tm_min);
    if(sleep_day <= 60 && sleep_day > 0){
        
        Serial.printf("Going to sleep day for %d minutes", sleep_day);
        ESP.deepSleep(sleep_day * 60 * 10 * 10 * 10 * 10 * 10 * 10);
    }else if(sleep_night <= 60 && sleep_night > 0){
        
        Serial.printf("Going to sleep night for %d minutes", sleep_night);
        ESP.deepSleep(sleep_night * 60 * 10 * 10 * 10 * 10 * 10 * 10);
    }else{
        
        Serial.printf("Going to sleep for %d minutes", 60 - getTime()->tm_min);
        ESP.deepSleep((60 - getTime()->tm_min) * 60 * 10 * 10 * 10 * 10 * 10 * 10);
    }
}

void buttonSetup(){
    
    while(millis() <= 60000){
        if(digitalRead(pushButtonUp) == HIGH && boiler0.getState() <= BL_5){
            boiler0.setState(boiler0.itostate(boiler0.statetoi(boiler0.getState()) + 1));
            boiler0.update();
            while(200 != setSettings());
            delay(500);
        }
        if(digitalRead(pushButtonDown) == HIGH && boiler0.getState() >= BL_1){
            boiler0.setState(boiler0.itostate(boiler0.statetoi(boiler0.getState()) - 1));
            boiler0.update();
            while(200 != setSettings());
            delay(500);
        }
    }
}

void setup() {
    Serial.begin(115200);
    wifiConnect();
    configTime(3600, 3600, "pool.ntp.org");
    
    servo1.attach(13);
    pinMode(pushButtonDown, INPUT);
    pinMode(pushButtonUp, INPUT);
    pinMode(LED, OUTPUT);
    
    esp_sleep_enable_ext0_wakeup((gpio_num_t)32, 1);
}

void loop() {
    switch(esp_sleep_get_wakeup_cause()){
        case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wake up by Button"); break;
        case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wake up by Timer"); break;
    }
    
    Serial.printf("Time %d:%d\n", getTime()->tm_hour, getTime()->tm_min);
    getSettings();
    
    buttonSetup();
    
    Serial.printf("Time %d:%d\n", getTime()->tm_hour, getTime()->tm_min);
    digitalWrite(LED, LOW);
    
    timedSleep();
    
}