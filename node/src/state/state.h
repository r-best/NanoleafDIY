/**
 * Responsible for storing/fetching the lighting data from EEPROM
 * 
 * Memory layout
 *  1 byte - Current mode
 *  3 bytes - Solid color data (r, g, b)
 *  1 byte - Length of custom gradient data
 *      
 */
#ifndef STATE_H
#define STATE_H

#include <EEPROM.h>

#include "../leds/leds.h"
#include "../leds/led_patterns.h"


uint8_t get_current_mode_state();

float get_brightness_state();

SolidColor* get_solid_color_state();

FadingGradient* get_custom_gradient_state();

Blink* get_blink_state();

void save_current_mode_state();

void save_brightness_state();

void save_solid_color_state();

void save_custom_gradient_state();

void save_blink_state();

#endif
