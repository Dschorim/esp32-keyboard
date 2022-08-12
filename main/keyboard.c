#include "keyboard.h"

uint8_t keys[0x100] = {0};
uint8_t pressed_keys[6] = {0};
uint8_t last_valid_state[6] = {0};
uint8_t num_pressed_keys = 0;
uint8_t mods = 0;

uint8_t press_key(uint8_t key) {
    if (keys[key]) { // return in case key is already pressed
        return 0;
    }
    keys[key] = 1;
    
    switch (key) {
        case HID_KEY_CONTROL_LEFT:
            mods |= (1 << 0);
            break;
        case HID_KEY_SHIFT_LEFT:
            mods |= (1 << 1);
            break;
        case HID_KEY_ALT_LEFT:
            mods |= (1 << 2);
            break;
        case HID_KEY_GUI_LEFT:
            mods |= (1 << 3);
            break;
        case HID_KEY_CONTROL_RIGHT:
            mods |= (1 << 4);
            break;
        case HID_KEY_SHIFT_RIGHT:
            mods |= (1 << 5);
            break;
        case HID_KEY_ALT_RIGHT:
            mods |= (1 << 6);
            break;
        case HID_KEY_GUI_RIGHT:
            mods |= (1 << 7);
            break;
        default:
            if (num_pressed_keys <= 6) { // can press up to 6 keys at once
                for (int i=0; i<6; i++) { // find the first free slot
                    if (pressed_keys[i] == HID_KEY_NONE) {
                        pressed_keys[i] = key;
                        memcpy(last_valid_state, pressed_keys, 6); // save the current state in case of error later on
                        num_pressed_keys++;
                        break;
                    }
                }
            } else { // error state
                for (int i=0; i < 6; i++) {
                    pressed_keys[i] = HID_KEY_ERROR;
                }
            }     
    }
    // print mods as hex
    printf("%x\n", mods);
    // print pressed keys
    for (int i=0; i<6; i++) {
        printf("%x ", pressed_keys[i]);
    }
    printf("\n");
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, mods, pressed_keys);
    return 0;
}

uint8_t release_key(uint8_t key) {
    if (!keys[key]) { // return in case key is already released
        return 0;
    }
    keys[key] = 0;

    switch (key) {
        case HID_KEY_CONTROL_LEFT:
            mods &= ~(1 << 0);
            break;
        case HID_KEY_SHIFT_LEFT:
            mods &= ~(1 << 1);
            break;
        case HID_KEY_ALT_LEFT:
            mods &= ~(1 << 2);
            break;
        case HID_KEY_GUI_LEFT:
            mods &= ~(1 << 3);
            break;
        case HID_KEY_CONTROL_RIGHT:
            mods &= ~(1 << 4);
            break;
        case HID_KEY_SHIFT_RIGHT:
            mods &= ~(1 << 5);
            break;
        case HID_KEY_ALT_RIGHT:
            mods &= ~(1 << 6);
            break;
        case HID_KEY_GUI_RIGHT:
            mods &= ~(1 << 7);
            break;
        default:
            if (num_pressed_keys == 7) { // recover from error state
                num_pressed_keys--;
                memcpy(pressed_keys, last_valid_state, 6);
            } else if (num_pressed_keys > 7) { // error state
                num_pressed_keys--;
            } else { // release a key
                for (int i=0; i<6; i++) {
                    if (pressed_keys[i] == key) {
                        num_pressed_keys--;
                        for (int j=i; j<5; j++) { // shift the remaining keys
                            pressed_keys[j] = pressed_keys[j+1];
                        }
                        pressed_keys[5] = HID_KEY_NONE;
                        break;
                    }
                }
            } 
    }
    printf("%x\n", mods);
    for (int i=0; i<6; i++) {
        printf("%x ", pressed_keys[i]);
    }
    printf("\n");
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, mods, pressed_keys);
    return 0;
}