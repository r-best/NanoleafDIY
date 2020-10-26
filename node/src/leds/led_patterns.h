#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

#include "../utils/constants.h"


struct ColorStep {
    uint8_t r, g, b;
    uint32_t time;

    ColorStep(uint8_t r, uint8_t g, uint8_t b, uint32_t time){
        this->r = r;
        this->g = g;
        this->b = b;
        this->time = time;
    }
    ColorStep(): ColorStep(0, 0, 0, 1){};
};
class Palette {
    ColorStep *steps;
    public:
        uint8_t length;
        bool randomize, synchronize;
        ColorStep operator[] (uint8_t x){ return steps[x]; }

        Palette(ColorStep *steps, uint8_t length, bool randomize, bool synchronize){
            this->steps = steps;
            this->length = length;
            this->randomize = randomize;
            this->synchronize = synchronize;
        }
};

extern Adafruit_NeoPixel leds;
extern float brightness;
extern Palette *palette;

static uint32_t color(uint8_t r, uint8_t g, uint8_t b){
    return leds.Color(r*brightness, g*brightness, b*brightness);
}

/** 
 * Base class for LED patterns, allows each
 * class to define state variables as necessary
 */
class Pattern {
    public:
        // Number of ms between each update of the LED strip
        int refresh_rate = 10;
        virtual void init();
        virtual void update();
};

/**
 * Fades between defined color steps over given time intervals
 * The color arrays r, g, and b store the desired color at each step in the gradient
 * (i.e. r[0] b[0] g[0] represents the starting color, r[1] g[1] b[1] is the second color, etc..)
 * and the transisitons array stores the time (in ms) it takes to fade between each step
 */
class FadeMode: public Pattern {
    unsigned long last_update; // Tracks current time in gradient
    uint8_t current_step; // The currently active step
    uint8_t next_step; // The next step (only used if randomize is enabled, otherwise next step is just next one in list)
    uint32_t timeToNextStep;
    public:
        void init() override {
            refresh_rate = 10;
            current_step = 0;
            next_step = palette->length > 1 ? 1 : 0;
            timeToNextStep = (*palette)[current_step].time;

            last_update = millis();
        }
        void update() override {
            // New color is the current step's color plus the fraction we are towards the next step's color based on elapsed time
            float elapsed_time = (float)(millis()-last_update) / (float)timeToNextStep;

            // If enough time has passed, advance to next step
            if(elapsed_time >= 1){
                // If randomize is enabled, choose a random step from the list instead of advancing to the next one
                if(palette->randomize){
                    current_step = next_step;
                    while(current_step == next_step)
                        next_step = random(0, palette->length);
                }
                else{
                    if(++current_step >= palette->length)
                        current_step = 0;
                    if(++next_step >= palette->length)
                        next_step = 0;
                }

                // If synchronize is enabled, use the correct time value, else select a different time value from the list randomly
                // This allows the arrangement of panels to not be changing colors in perfect sync
                if(palette->synchronize) timeToNextStep = (*palette)[current_step].time;
                else            timeToNextStep = (*palette)[random(0, palette->length)].time;

                elapsed_time = 0;
                last_update = millis();
            }

            uint8_t newR = (*palette)[current_step].r + elapsed_time*((*palette)[next_step].r - (*palette)[current_step].r);
            uint8_t newG = (*palette)[current_step].g + elapsed_time*((*palette)[next_step].g - (*palette)[current_step].g);
            uint8_t newB = (*palette)[current_step].b + elapsed_time*((*palette)[next_step].b - (*palette)[current_step].b);

            leds.fill(color(newR, newG, newB));
        }
};

/**
 * Similar to FadeMode (see above), but blinks to the next color in the sequence instead
 * of gradually fading between them
 * The color arrays r, g, and b store the desired color at each step in the sequence
 * (i.e. r[0] b[0] g[0] represents the starting color, r[1] g[1] b[1] is the second color, etc..)
 * and the transisitons array stores the time (in ms) to wait until advancing to the next step
 */
class BlinkMode: public Pattern {
    unsigned long last_update; // Tracks current time
    uint8_t current_step; // The currently active step
    uint32_t timeToNextStep;
    public:

        void init() override {
            refresh_rate = 10;
            current_step = 0;
            timeToNextStep = (*palette)[current_step].time;

            last_update = millis();
        }
        void update() override {
            // If enough time has passed, advance to next step
            unsigned long now = millis();
            if(now-last_update >= timeToNextStep){
                // If randomize is enabled, choose a random next step, else advance to next step in list
                if(palette->randomize){
                    uint8_t old = current_step;
                    while(old == current_step)
                        current_step = random(0, palette->length);
                }
                else{
                    if(++current_step >= palette->length)
                        current_step = 0;
                }

                // If synchronize is enabled, use the correct time value, else select a different time value from the list randomly
                // This allows the arrangement of panels to not be changing colors in perfect sync
                if(palette->synchronize) timeToNextStep = (*palette)[current_step].time;
                else            timeToNextStep = (*palette)[random(0, palette->length)].time;

                last_update = now;
                leds.fill(color((*palette)[current_step].r, (*palette)[current_step].g, (*palette)[current_step].b));
            }
        }
};

/**
 * Applies a rainbow effect scrolling across the LEDs
 * 
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
class RainbowMode: public Pattern {
    unsigned short hue;
    public:
        void init() override { refresh_rate = 10; hue = 0; }
        void update() override {
            // Offset each LED from the current hue by the full color wheel size / NUM_LEDS
            // This results in all the colors of the wheel being spaced across the LEDs
            for(int i=0; i<NUM_LEDS; i++) 
                leds.setPixelColor(i,
                    leds.gamma32(leds.ColorHSV(
                        hue + (i * 65536L / NUM_LEDS)
                    ))*brightness
                );
            hue += 256;
        }
};

/**
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
class TheaterChase: public Pattern {
    int offset;
    public:
        void init() override { refresh_rate = 300; offset = 0; }
        void update() override {
            leds.clear();
            for(int i = offset; i < leds.numPixels(); i += 3)
                leds.setPixelColor(i, color(255, 255, 255));
            offset < 2 ? offset++ : offset = 0;
        }
};

/**
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
class TheaterChaseRainbow: public Pattern {
    unsigned short hue;
    int offset;
    public:
        void init() override { refresh_rate = 300; hue = 0; offset = 0; }
        void update() override {
            leds.clear();
            for(int i = offset; i < leds.numPixels(); i += 3)
                leds.setPixelColor(i, 
                    leds.gamma32(leds.ColorHSV(
                        hue + i * 65536L / NUM_LEDS
                    ))*brightness
                );
            offset < 2 ? offset++ : offset = 0;
            hue += 65536 / 90;
        }
};

#endif
