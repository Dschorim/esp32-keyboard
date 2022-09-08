// This files contains all the configuration options for the keyboard

#pragma once

#include "patterns.h"
#include "class/hid/hid.h"

#define portTICK_PERIOD_US              ( ( TickType_t ) 1000000 / configTICK_RATE_HZ )

//************* LED stuff *****************//
#define LED_PIN     48
#define LED_NUM     1
#define PATTERN     PATTERN_STATIC
#define PATTERN_COL 0x004000

//*********** internal stuff ***************//
#define STACK_SIZE 4096

//************* key stuff *****************//

#define ROW_COUNT       2
#define COLUMN_COUNT    2
uint8_t ROW_PINS[ROW_COUNT] = {7, 15};
uint8_t COLUMN_PINS[COLUMN_COUNT] = {5, 6};

uint8_t KEYS[ROW_COUNT][COLUMN_COUNT] = {
    {HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP},
    {HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT}
};

//rise time in us
#define RISE_TIME       20
