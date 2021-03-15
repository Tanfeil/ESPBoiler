//
// Created by jonat on 22.01.2021.
//

#ifndef BOILER_H
#define BOILER_H


#include "bl_definitions.h"
#include "../.pio/libdeps/esp32dev/ServoESP32/src/Servo.h"

class Boiler {
private:
    int status;
    bl_state state;
    bl_time night;
    bl_time day;
    tm* (*time)();
    Servo* servo;

    int checkStatus();
    bl_state getActualState();
    void setServo();

    void setDay(int hours, int minutes, bl_state _state);
    void setNight(int hours, int minutes, bl_state _state);
public:
    Boiler(tm* (*_time)(), Servo* _servo);

    int getStateDegree() const;
    bl_state getState() const;
    bl_state getStateNight();
    bl_state getStateDay();
    int getHoursDay();
    int getMinutesDay();
    int getHoursNight();
    int getMinutesNight();
    int getStatus() const;
    tm* getTime();

    void update();

    void setTimeDay(int hours, int minutes);
    void setTimeNight(int hours, int minutes);

    void setStateNight(bl_state _state);
    void setStateDay(bl_state _state);

    void setState(bl_state _state);

    static int statetoi(bl_state tmp);
    static bl_state itostate(int tmp);
};

#endif //UNTITLED_BOILER_H
