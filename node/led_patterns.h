#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

/** 
 * Base class for LED patterns, allows each
 * class to define state variables as necessary
 */
class Pattern {
    public:
        virtual void init();
        virtual void update();
};

/**
 * 
 */
class CustomGradient: public Pattern {
    // Tracks current time in gradient
    unsigned long last_update;
    public:
        // The number of steps in the gradient
        uint8_t length;
        // Steps in color gradient, i.e. r[0] b[0] g[0] represents the starting color
        uint8_t *r = malloc(sizeof(uint8_t));
        uint8_t *g = malloc(sizeof(uint8_t));
        uint8_t *b = malloc(sizeof(uint8_t));
        // Time between each color step, i.e. transitions[0] is
        // the transisiton time from step 0 to step 1
        uint32_t *transitions = malloc(sizeof(uint32_t));

        void destroy() {
            free(r);
            free(g);
            free(b);
            free(transitions);
        }

        void init() override {

        }
        void update() override {

        }
} CustomGradient;

/**
 * Applies a rainbow effect scrolling across the LEDs
 * 
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
class Rainbow: public Pattern {
    unsigned short hue;
    public:
        void init() override { hue = 0; }
        void update() override {
            // Offset each LED from the current hue by the full color wheel size / NUM_LEDS
            // This results in all the colors of the wheel being spaced across the LEDs
            for(int i=0; i<NUM_LEDS; i++) 
                leds.setPixelColor(i,
                    leds.gamma32(leds.ColorHSV(
                        hue + (i * 65536L / NUM_LEDS)
                    ))
                );
            hue += 256;
        }
} Rainbow;

/**
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
class TheaterChase: public Pattern {
    int offset;
    public:
        void init() override { offset = 0; }
        void update() override {
            leds.clear();
            for(int i = offset; i < leds.numPixels(); i += 3)
                leds.setPixelColor(i, leds.Color(255, 255, 255));
            offset < 2 ? offset++ : offset = 0;
        }
} TheaterChase;

/**
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
class TheaterChaseRainbow: public Pattern {
    unsigned short hue;
    int offset;
    public:
        void init() override { hue = 0; offset = 0; }
        void update() override {
            leds.clear();
            for(int i = offset; i < leds.numPixels(); i += 3)
                leds.setPixelColor(i, 
                    leds.gamma32(leds.ColorHSV(
                        hue + i * 65536L / NUM_LEDS
                    ))
                );
            offset < 2 ? offset++ : offset = 0;
            hue += 65536 / 90;
        }
} TheaterChaseRainbow;

#endif
