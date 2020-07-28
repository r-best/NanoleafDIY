/**
 * This class controls the led strip
 */
#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>

#include "../utils/constants.h"


void setup_leds();

/** Updates the LEDs according to the active pattern, called in arduino loop */
void update_leds();

/** Adjusts how often (in ms) the LEDs are updated */
void set_refresh_rate(uint8_t ms);

/**
 * Sets the current pattern to be displayed on the panel
 * `mode` must be a valid index of the `MODES` array
 */
int set_mode(uint8_t mode);

/** Updates the stored custom gradient with the given parameters */
int set_custom_gradient(uint8_t length, uint8_t *r, uint8_t *g, uint8_t *b, uint32_t *transitions);

/** Sets the panel to one solid color, disabling any active pattern */
int set_solid(uint8_t r, uint8_t g, uint8_t b);

#endif
