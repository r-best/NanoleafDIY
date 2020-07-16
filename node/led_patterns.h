#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

Adafruit_NeoPixel leds(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

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
 * 
 */
class FadingGradient: public Pattern {
    // Tracks current time in gradient
    unsigned long last_update;
    public:
        // The current step
        uint8_t current_step;
        // The number of steps in the gradient
        uint8_t length;
        // Steps in color gradient, i.e. r[0] b[0] g[0] represents the starting color
        uint8_t *r, *g, *b;
        // Time between each color step, i.e. transitions[0] is
        // the transisiton time (in ms) from step 0 to step 1
        uint32_t *transitions;

        void init() override { refresh_rate = 10; };
        void update() override {
            // New color is the current step's color plus the fraction we are towards the next step's color based on elapsed time
            float elapsed_time = (float)(millis()-last_update) / (float)transitions[current_step];

            // If enough time has passed, advance to next step
            if(elapsed_time >= 1){
                if(++current_step >= length)
                    current_step = 0;
                elapsed_time = 0;
                last_update = millis();
            }

            uint8_t nextStep = current_step == length-1 ? 0 : current_step+1;
            uint8_t newR = r[current_step] + elapsed_time*(r[nextStep] - r[current_step]);
            uint8_t newG = g[current_step] + elapsed_time*(g[nextStep] - g[current_step]);
            uint8_t newB = b[current_step] + elapsed_time*(b[nextStep] - b[current_step]);

            leds.fill(leds.Color(newR, newG, newB));
        }

        FadingGradient(uint8_t length, uint8_t *r, uint8_t *g, uint8_t *b, uint32_t *transitions) {
            this->length = length;
            this->r = r;
            this->g = g;
            this->b = b;
            this->transitions = transitions;

            this->current_step = 0;
            this->last_update = millis();
        }

        ~FadingGradient() {
            free(r);
            free(g);
            free(b);
            free(transitions);
        }
};

/**
 * Applies a rainbow effect scrolling across the LEDs
 * 
 * Adapted from Adafruit Neopixel library examples
 * https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/buttoncycler/buttoncycler.ino
 */
class RainbowPattern: public Pattern {
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
                    ))
                );
            hue += 256;
        }
        RainbowPattern() {
            this->init();
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
                leds.setPixelColor(i, leds.Color(255, 255, 255));
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
                    ))
                );
            offset < 2 ? offset++ : offset = 0;
            hue += 65536 / 90;
        }
};

#endif
