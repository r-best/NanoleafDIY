/**
 * This class controls the led strip
 */
#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>

#include "led_patterns.h"

#include "../utils/constants.h"

extern uint8_t current_mode;
extern Pattern *MODES[];

void setup_leds();

/** Updates the LEDs according to the active pattern, called in arduino loop */
void update_leds();

/** Adjusts the brightness of the LEDs, takes a decimal value from 0.0-1.0 */
void set_brightness(float brightness);

/**
 * Sets the current pattern to be displayed on the panel
 * `mode` must be a valid index of the `MODES` array
 */
int set_mode(uint8_t mode);

/** Updates the stored custom gradient with the given parameters */
void set_color_state(ColorStep* steps, uint8_t length);

#endif
