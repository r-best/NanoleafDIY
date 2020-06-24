/**
 * This class controls the led strip
 */
#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>


Adafruit_NeoPixel leds(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
boolean do_led_update = true;

uint8_t current_mode = 0;
int update_rate = 10;
unsigned long timer;

/**
 * Applies a rainbow effect scrolling across the LEDs
 * 
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
unsigned short hue = 0;
void rainbow() {
    // Offset each LED from the current hue by the full color wheel size / NUM_LEDS
    // This results in all the colors of the wheel being spaced across the LEDs
    for(int i=0; i<NUM_LEDS; i++) 
        leds.setPixelColor(i,
            leds.gamma32(leds.ColorHSV(
                hue + (i * 65536L / NUM_LEDS)
            ))
        );
    hue += 256;
    leds.show();
}

/**
 * 
 * 
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
int offset = 0;
void theaterChase() {
    leds.clear();
    for(int i = offset; i < leds.numPixels(); i += 3)
        leds.setPixelColor(i, leds.Color(255, 255, 255));
    offset < 2 ? offset++ : offset = 0;
    leds.show();
}

/**
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
void theaterChaseRainbow() {
    leds.clear();
    for(int i = offset; i < leds.numPixels(); i += 3)
        leds.setPixelColor(i, 
            leds.gamma32(leds.ColorHSV(
                hue + i * 65536L / NUM_LEDS
            ))
        );
    offset < 2 ? offset++ : offset = 0;
    hue += 65536 / 90;
    leds.show();
}

void(*MODES[])(void) = {
    &rainbow,
    &theaterChase,
    &theaterChaseRainbow
};

int update_leds(){
    if(do_led_update && millis() - timer >= update_rate){
        MODES[current_mode]();
        timer = millis();
    }
    return 0;
}

int set_solid(uint8_t r, uint8_t g, uint8_t b){
    do_led_update = false;
    leds.fill(leds.Color(r, g, b));
    leds.show();
    return 0;
}

int set_mode(uint8_t mode){
    do_led_update = true;
    current_mode = mode;
    return 0;
}

int set_update_rate(uint8_t s){
    update_rate = s;
    return 0;
}

#endif
