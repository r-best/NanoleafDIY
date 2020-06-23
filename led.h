/**
 * This class controls the led strip
 */
#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>


Adafruit_NeoPixel leds(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
uint8_t ar=0, ag=0, ab=0;

int update(){
    leds.clear();
    for(int i = 0; i < NUM_LEDS; i++){
        leds.setPixelColor(i, leds.Color(ar, ag, ab));
        leds.show();
    }
    return 0;
}

int set_solid(uint8_t r, uint8_t g, uint8_t b){
    ar = r;
    ag = g;
    ab = b;
    return update();
}

#endif
