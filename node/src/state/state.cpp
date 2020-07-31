#include "state.h"


uint8_t get_current_mode_state(){
    uint8_t mode;
    EEPROM.get(0, mode);
    return mode;
}

SolidColor* get_solid_color_state(){
    SolidColor *solidColor = new SolidColor(0,0,0);
    EEPROM.get(1,                       solidColor->r);
    EEPROM.get(1 + sizeof(uint8_t),     solidColor->g);
    EEPROM.get(1 + sizeof(uint8_t)*2,   solidColor->b);

    return solidColor;
}

FadingGradient* get_custom_gradient_state(){
    uint32_t offset = 1 + sizeof(uint8_t) * 3;

    uint8_t length;
    EEPROM.get(offset, length);
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

void save_current_mode_state(){
    EEPROM.put(0, current_mode);
}

void save_solid_color_state(){
    SolidColor *solidColor = (SolidColor*)MODES[0];

    EEPROM.put(1,                   solidColor->r);
    EEPROM.put(1 + sizeof(uint8_t),     solidColor->g);
    EEPROM.put(1 + sizeof(uint8_t)*2,   solidColor->b);
}

void save_custom_gradient_state(){
    uint32_t offset = 1 + sizeof(uint8_t) * 3;

    FadingGradient *fadingGradient = (FadingGradient*)MODES[1];

    EEPROM.put(offset, fadingGradient->length);
    offset += sizeof(uint8_t);

    for(int i = 0; i < fadingGradient->length; i++){
        EEPROM.put(offset,                      fadingGradient->r[i]);
        EEPROM.put(offset + sizeof(uint8_t),    fadingGradient->g[i]);
        EEPROM.put(offset + sizeof(uint8_t)*2,  fadingGradient->b[i]);
        EEPROM.put(offset + sizeof(uint8_t)*3,  fadingGradient->transitions[i]);
        offset += sizeof(uint8_t)*3 + sizeof(uint32_t);
    }
}
