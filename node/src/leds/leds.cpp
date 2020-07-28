#include "leds.h"
#include "led_patterns.h"


Adafruit_NeoPixel leds(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

// True if a pattern is being displayed, i.e. we need to continuously update leds
// False if we're just displaying a solid color, i.e. no updates
bool running_pattern = true;
// Index of the currently selected builtin pattern
uint8_t current_mode = 0;
unsigned long timer;

/** List of color modes, implementations in `led_patterns.h` */
Pattern *MODES[] = {
    new FadingGradient(),
    new RainbowPattern(),
    new TheaterChase(),
    new TheaterChaseRainbow()
};

void setup_leds(){
    leds.begin();
    leds.setBrightness(25);
    timer = millis();
}

void update_leds(){
    // Only perform an update if we're running a pattern and the
    // correct amount of time has passed since the last update
    if(running_pattern && millis() - timer >= MODES[current_mode]->refresh_rate){
        MODES[current_mode]->update();
        leds.show();
        timer = millis();
    }
}

void set_refresh_rate(uint8_t ms){ MODES[current_mode]->refresh_rate = ms; }

int set_mode(uint8_t mode){
    running_pattern = true;
    MODES[mode]->init();
    current_mode = mode;
    return 0;
}

int set_custom_gradient(uint8_t length, uint8_t *r, uint8_t *g, uint8_t *b, uint32_t *transitions){
    delete MODES[0];
    MODES[0] = new FadingGradient(length, r, g, b, transitions);
}

int set_solid(uint8_t r, uint8_t g, uint8_t b){
    running_pattern = false;
    leds.fill(leds.Color(r, g, b));
    leds.show();
    return 0;
}
