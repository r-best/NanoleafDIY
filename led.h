/**
 * This class controls the led strip
 */
#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>


Adafruit_NeoPixel leds(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
boolean leds_active = true;

uint8_t current_mode = 1;
uint8_t speed = 50;
unsigned long timer;

/**
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 * Need to fix delay issue
 */
void rainbow() {
    for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
        for(int i=0; i<NUM_LEDS; i++) {
            int pixelHue = firstPixelHue + (i * 65536L / NUM_LEDS);
            leds.setPixelColor(i, leds.gamma32(leds.ColorHSV(pixelHue)));
        }
        leds.show();
    }
}

/**
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
void theaterChaseRainbow() {
    int firstPixelHue = 0;
    for(int a = 0; a < 30; a++) {
        for(int b = 0; b < 3; b++) {
            for(int c = b; c < leds.numPixels(); c += 3) {
                int hue = firstPixelHue + c * 65536L / leds.numPixels();
                uint32_t color = leds.gamma32(leds.ColorHSV(hue));
                leds.setPixelColor(c, color);
            }
            leds.show();
            delay(speed);
            firstPixelHue += 65536 / 90;
        }
    }
}

void(*MODES[])(void) = {
    &rainbow,
    &theaterChaseRainbow
};

int update_leds(){
    // if(leds_active && millis() - timer >= speed)
        MODES[current_mode]();
    return 0;
}

int set_solid(uint8_t r, uint8_t g, uint8_t b){
    leds_active = false;
    leds.fill(leds.Color(r, g, b));
    leds.show();
    return 0;
}

int set_mode(uint8_t mode){
    leds_active = true;
    current_mode = mode;
    return 0;
}

int set_speed(uint8_t s){
    speed = s;
    return 0;
}

#endif
