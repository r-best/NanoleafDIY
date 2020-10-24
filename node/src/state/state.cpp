#include "state.h"

// Sizes of different objects in memory, to compute positions to write objects to
const uint8_t MODE_SIZE = sizeof(uint8_t);
const uint8_t BRIGHTNESS_SIZE = sizeof(float);
uint8_t COLOR_STATE_SIZE = 0; // Updated on reading/writing color state save data


uint8_t get_current_mode_state(){
    uint8_t mode;
    EEPROM.get(0, mode);
    return mode;
}

float get_brightness_state(){
    uint32_t offset = MODE_SIZE;

    float brightness;
    EEPROM.get(offset, brightness);
    return brightness;
}

Palette* get_color_state(){
    uint32_t offset = MODE_SIZE + BRIGHTNESS_SIZE;

    uint8_t length;
    EEPROM.get(offset, length);
    COLOR_STATE_SIZE = sizeof(uint8_t) + (sizeof(uint8_t)*3 + sizeof(uint32_t)) * length;
    offset += sizeof(uint8_t);

    ColorStep *steps = (ColorStep*)malloc(sizeof(ColorStep)*length);
    for(int i = 0; i < length; i++){
        steps[i] = ColorStep();
        EEPROM.get(offset,                      steps[i].r);
        EEPROM.get(offset + sizeof(uint8_t),    steps[i].g);
        EEPROM.get(offset + sizeof(uint8_t)*2,  steps[i].b);
        EEPROM.get(offset + sizeof(uint8_t)*3,  steps[i].time);
        offset += sizeof(uint8_t)*3 + sizeof(uint32_t);
    }

    return new Palette(steps, length);
}

void save_current_mode_state(){
    EEPROM.put(0, current_mode);
}

void save_brightness_state(){
    uint32_t offset = MODE_SIZE;
    EEPROM.put(offset, brightness);
}

void save_color_state(){
    uint32_t offset = MODE_SIZE + BRIGHTNESS_SIZE;

    EEPROM.put(offset, palette->length);
    COLOR_STATE_SIZE = sizeof(uint8_t) + (sizeof(uint8_t)*3 + sizeof(uint32_t)) * palette->length;
    offset += sizeof(uint8_t);

    for(int i = 0; i < palette->length; i++){
        EEPROM.put(offset,                      (*palette)[i].r);
        EEPROM.put(offset + sizeof(uint8_t),    (*palette)[i].g);
        EEPROM.put(offset + sizeof(uint8_t)*2,  (*palette)[i].b);
        EEPROM.put(offset + sizeof(uint8_t)*3,  (*palette)[i].time);
        offset += sizeof(uint8_t)*3 + sizeof(uint32_t);
    }
}
