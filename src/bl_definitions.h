//
// Created by jonat on 22.01.2021.
//

#ifndef BL_DEFINITIONS_H
#define BL_DEFINITIONS_H

/*
 * Boiler rotary control states for servo
 */
typedef enum {
    BL_0 = 0,
    BL_1 = 30,
    BL_2 = 60,
    BL_3 = 90,
    BL_4 = 120,
    BL_5 = 150,
    BL_6 = 180,
    BL_7 = 210,
    BL_8 = 240,
    BL_9 = 270,
    BL_10 = 300,
    BL_11 = 330,
    BL_12 = 360
}bl_state;

typedef enum {
    NIGHT = 0,
    DAY = 1
}bl_status;

typedef struct {
    int hours;
    int minutes;
    bl_state state;
}bl_time;

#endif //UNTITLED_BL_DEFINITIONS_H
