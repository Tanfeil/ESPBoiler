#include <algorithm>
//
// Created by jonat on 22.01.2021.
//

#include "Boiler.h"
#include <time.h>


Boiler::Boiler(tm* (*_time)(), Servo* _servo) {
    time = _time;

    servo = _servo;
    setDay(9, 0, BL_0);
    setNight(22, 0, BL_0);

    status = checkStatus();
    state = getActualState();

    setServo();
}

void Boiler::update(){
    status = checkStatus();
    state = getActualState();
    Serial.printf("%d -> %d\n", servo->read(), state);
    setServo();
}


int Boiler::checkStatus(){
    bool _night = (*time)()->tm_hour == night.hours && (*time)()->tm_min >= night.minutes ;
    bool _day = (*time)()->tm_hour == day.hours && (*time)()->tm_min <= day.minutes;
    if(_day || _night || (*time)()->tm_hour > night.hours || (*time)()->tm_hour < day.hours) return NIGHT;
    else return DAY;
}

bl_state Boiler::getActualState(){
    if(status == NIGHT) return night.state;
    else return day.state;
}

int Boiler::getStateDegree() const {
    return statetoi(getState()) * 30;
}

bl_state Boiler::getState() const {
    return state;
}

int Boiler::getStatus() const {
    return status;
}

tm* Boiler::getTime() {
    return (*time)();
}

void Boiler::setServo() {
    servo->write((int)state);
}

void Boiler::setNight(int hours, int minutes, bl_state _state){
    setTimeNight(hours, minutes);
    setStateNight(_state);
}

void Boiler::setDay(int hours, int minutes, bl_state _state){
    setTimeDay(hours, minutes);
    setStateDay(_state);
}

void Boiler::setTimeDay(int hours, int minutes){
    day.hours = hours;
    day.minutes = minutes;
}

void Boiler::setTimeNight(int hours, int minutes){
    night.hours = hours;
    night.minutes = minutes;
}

void Boiler::setStateDay(bl_state _state){
    day.state = _state;
};

void Boiler::setStateNight(bl_state _state){
    night.state = _state;
};

int Boiler::statetoi(bl_state tmp){
    switch(tmp){
        case BL_0: return 0;
        case BL_1: return 1;
        case BL_2: return 2;
        case BL_3: return 3;
        case BL_4: return 4;
        case BL_5: return 5;
        case BL_6: return 6;
        case BL_7: return 7;
        case BL_8: return 8;
        case BL_9: return 9;
        case BL_10: return 10;
        case BL_11: return 11;
        case BL_12: return 12;
        default: return -1;
    }
}

bl_state Boiler::itostate(int tmp){
    switch(tmp){
        case 0: return BL_0;
        case 1: return BL_1;
        case 2: return BL_2;
        case 3: return BL_3;
        case 4: return BL_4;
        case 5: return BL_5;
        case 6: return BL_6;
        case 7: return BL_7;
        case 8: return BL_8;
        case 9: return BL_9;
        case 10: return BL_10;
        case 11: return BL_11;
        case 12: return BL_12;
    }
}

void Boiler::setState(bl_state _state){
    if(status == NIGHT) night.state = _state;
    else day.state = _state;
}

bl_state Boiler::getStateNight() {
    return night.state;
}

bl_state Boiler::getStateDay() {
    return day.state;
}

int Boiler::getHoursDay() {
    return day.hours;
}

int Boiler::getMinutesDay() {
    return day.minutes;
}

int Boiler::getHoursNight() {
    return night.hours;
}

int Boiler::getMinutesNight() {
    return night.minutes;
}
