#pragma once

#include <stdint.h>
#include "class/hid/hid.h"
#include "descriptors_control.h"

#define HID_KEY_ERROR 0x01

uint8_t press_key(uint8_t key);
uint8_t release_key(uint8_t key);
