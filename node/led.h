/**
 * This class controls the led strip
 */
#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>

#include "led_patterns.h"


// True if a pattern is being displayed, i.e. we need to continuously update leds
// False if we're just displaying a solid color, i.e. no updates
boolean running_pattern = true;
// Index of the currently selected builtin pattern
uint8_t current_mode = 0;
unsigned long timer;

/** List of predefined color patterns, implementations in `led_patterns.h` */
Pattern *MODES[] = {
    NULL, // Slot for user-defined gradient, initialized in `setup_leds()`
    new RainbowPattern(),
    new TheaterChase(),
    new TheaterChaseRainbow()
};

/** Adjusts how often (in ms) the LEDs are updated */
void set_refresh_rate(uint8_t ms){ MODES[current_mode]->refresh_rate = ms; }

void setup_leds(){
    // Create default custom gradient
    uint8_t r[] = { 0, 255 };
    uint8_t g[] = { 0, 147 };
    uint8_t b[] = { 0, 41 };
    uint32_t transitions[] = { 1000, 1000 };
    MODES[0] = new FadingGradient(2, r, g, b, transitions);

    leds.begin();
    leds.setBrightness(25);
    timer = millis();
}

/** Updates the LEDs according to the active pattern, called in arduino loop */
void update_leds(){
    // Only perform an update if we're running a pattern and the
    // correct amount of time has passed since the last update
    if(running_pattern && millis() - timer >= MODES[current_mode]->refresh_rate){
        MODES[current_mode]->update();
        leds.show();
        timer = millis();
    }
}

/**
 * Sets the current pattern to be displayed on the panel
 * `mode` must be a valid index of the `MODES` array
 */
int set_mode(uint8_t mode){
    running_pattern = true;
    MODES[mode]->init();
    current_mode = mode;
    return 0;
}

/** Updates the stored custom gradient with the given parameters */
int set_custom_gradient(uint8_t length, uint8_t *r, uint8_t *g, uint8_t *b, uint32_t *transitions){
    delete MODES[0];
    MODES[0] = new FadingGradient(length, r, g, b, transitions);
}

/** Sets the panel to one solid color, disabling any active pattern */
int set_solid(uint8_t r, uint8_t g, uint8_t b){
    running_pattern = false;
    leds.fill(leds.Color(r, g, b));
    leds.show();
    return 0;
}

#endif
