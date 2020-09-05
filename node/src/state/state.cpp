#include "state.h"

// Sizes of different objects in memory, to compute positions to write objects to
const uint8_t MODE_SIZE = sizeof(uint8_t);
const uint8_t BRIGHTNESS_SIZE = sizeof(float);
const uint8_t SOLID_COLOR_SIZE = sizeof(uint8_t)*3;
uint8_t GRADIENT_SIZE = 0; // Updated on reading/writing gradient save data
uint8_t BLINK_SIZE = 0; // Updated on reading/writing blink data


uint8_t get_current_mode_state(){
    uint8_t mode;
    EEPROM.get(0, mode);
    return mode;
}

float get_brightness_state(){
    uint32_t offset = MODE_SIZE;

    float mode;
    EEPROM.get(offset, mode);
    Serial.println(mode);
    return mode;
}

SolidColor* get_solid_color_state(){
    uint32_t offset = MODE_SIZE + BRIGHTNESS_SIZE;

    SolidColor *solidColor = new SolidColor(0,0,0);
    EEPROM.get(offset,                       solidColor->r);
    EEPROM.get(offset + sizeof(uint8_t),     solidColor->g);
    EEPROM.get(offset + sizeof(uint8_t)*2,   solidColor->b);

    return solidColor;
}

FadingGradient* get_custom_gradient_state(){
    uint32_t offset = MODE_SIZE + BRIGHTNESS_SIZE + SOLID_COLOR_SIZE;

    uint8_t length;
    EEPROM.get(offset, length);
    GRADIENT_SIZE = sizeof(uint8_t) + (sizeof(uint8_t)*3 + sizeof(uint32_t)) * length;
    offset += sizeof(uint8_t);

    FadingGradient* fadingGradient = new FadingGradient(length);

    for(int i = 0; i < fadingGradient->length; i++){
        EEPROM.get(offset,                      fadingGradient->r[i]);
        EEPROM.get(offset + sizeof(uint8_t),    fadingGradient->g[i]);
        EEPROM.get(offset + sizeof(uint8_t)*2,  fadingGradient->b[i]);
        EEPROM.get(offset + sizeof(uint8_t)*3,  fadingGradient->transitions[i]);
        offset += sizeof(uint8_t)*3 + sizeof(uint32_t);
    }

    return fadingGradient;
}

Blink* get_blink_state(){
    uint32_t offset = MODE_SIZE + BRIGHTNESS_SIZE + SOLID_COLOR_SIZE + GRADIENT_SIZE;

    uint8_t length;
    EEPROM.get(offset, length);
    BLINK_SIZE = sizeof(uint8_t) + (sizeof(uint8_t)*3 + sizeof(uint32_t)) * length;
    offset += sizeof(uint8_t);

    Blink* blink = new Blink(length);

    for(int i = 0; i < blink->length; i++){
        EEPROM.get(offset,                      blink->r[i]);
        EEPROM.get(offset + sizeof(uint8_t),    blink->g[i]);
        EEPROM.get(offset + sizeof(uint8_t)*2,  blink->b[i]);
        EEPROM.get(offset + sizeof(uint8_t)*3,  blink->transitions[i]);
        offset += sizeof(uint8_t)*3 + sizeof(uint32_t);
    }

    return blink;
}

void save_current_mode_state(){
    EEPROM.put(0, current_mode);
}

void save_brightness_state(){
    uint32_t offset = MODE_SIZE;
    EEPROM.put(offset, brightness);
}

void save_solid_color_state(){
    uint32_t offset = MODE_SIZE + BRIGHTNESS_SIZE;

    SolidColor *solidColor = (SolidColor*)MODES[0];

    EEPROM.put(offset,                      solidColor->r);
    EEPROM.put(offset + sizeof(uint8_t),    solidColor->g);
    EEPROM.put(offset + sizeof(uint8_t)*2,  solidColor->b);
}

void save_custom_gradient_state(){
    uint32_t offset = MODE_SIZE + BRIGHTNESS_SIZE + SOLID_COLOR_SIZE;

    FadingGradient *fadingGradient = (FadingGradient*)MODES[1];

    EEPROM.put(offset, fadingGradient->length);
    GRADIENT_SIZE = sizeof(uint8_t) + (sizeof(uint8_t)*3 + sizeof(uint32_t)) * fadingGradient->length;
    offset += sizeof(uint8_t);

    for(int i = 0; i < fadingGradient->length; i++){
        EEPROM.put(offset,                      fadingGradient->r[i]);
        EEPROM.put(offset + sizeof(uint8_t),    fadingGradient->g[i]);
        EEPROM.put(offset + sizeof(uint8_t)*2,  fadingGradient->b[i]);
        EEPROM.put(offset + sizeof(uint8_t)*3,  fadingGradient->transitions[i]);
        offset += sizeof(uint8_t)*3 + sizeof(uint32_t);
    }
}

void save_blink_state(){
    uint32_t offset = MODE_SIZE + BRIGHTNESS_SIZE + SOLID_COLOR_SIZE + GRADIENT_SIZE;

    Blink *blink = (Blink*)MODES[2];

    EEPROM.put(offset, blink->length);
    BLINK_SIZE = sizeof(uint8_t) + (sizeof(uint8_t)*3 + sizeof(uint32_t)) * blink->length;
    offset += sizeof(uint8_t);

    for(int i = 0; i < blink->length; i++){
        EEPROM.put(offset,                      blink->r[i]);
        EEPROM.put(offset + sizeof(uint8_t),    blink->g[i]);
        EEPROM.put(offset + sizeof(uint8_t)*2,  blink->b[i]);
        EEPROM.put(offset + sizeof(uint8_t)*3,  blink->transitions[i]);
        offset += sizeof(uint8_t)*3 + sizeof(uint32_t);
    }
}
