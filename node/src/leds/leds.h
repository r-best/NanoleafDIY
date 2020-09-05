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

/** Sets the panel to one solid color, disabling any active pattern */
void set_solid(uint8_t r, uint8_t g, uint8_t b);

/** Updates the stored custom gradient with the given parameters */
void set_custom_gradient(uint8_t length, uint8_t *r, uint8_t *g, uint8_t *b, uint32_t *transitions);

/** Updates the stored blink pattern with the given parameters */
void set_blink(uint8_t length, uint8_t *r, uint8_t *g, uint8_t *b, uint32_t *transitions);

#endif
