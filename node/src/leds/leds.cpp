#include "leds.h"

#include "../state/state.h"


Adafruit_NeoPixel leds(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

// Index of the currently selected builtin pattern
uint8_t current_mode = 0;
int brightness = 255;
float brightness_mult = 1;
unsigned long timer;

// Current color set, 
Palette *palette = new Palette(NULL, 0, false, true);

/** List of color modes, implementations in `led_patterns.h` */
Pattern *MODES[] = {
    new FadeMode(),
    new BlinkMode(),
    new RainbowMode(),
    new TheaterChase(),
    new TheaterChaseRainbow()
};

void setup_leds(){
    leds.begin();

    // Load lighting state from EEPROM
    palette = get_color_state();
    set_brightness(get_brightness_state());
    set_mode(get_current_mode_state());

    timer = millis();
}

void update_leds(){
    // Only perform an update if the active pattern's refresh_rate is
    // nonnegative and the correct amount of time has passed since the last update
    if(MODES[current_mode]->refresh_rate > -1 && millis() - timer >= MODES[current_mode]->refresh_rate){
        MODES[current_mode]->update();
        leds.show();
        timer = millis();
    }
}

void set_brightness(int x){
    brightness = x;
    brightness_mult = x/255.0;
    MODES[current_mode]->init();
    save_brightness_state();
}

int set_mode(uint8_t mode){
    MODES[mode]->init();
    current_mode = mode;
    save_current_mode_state();
    return 0;
}

void set_color_state(ColorStep *steps, uint8_t length, bool randomize, bool synchronize){
    delete palette;
    palette = new Palette(steps, length, randomize, synchronize);
    MODES[current_mode]->init();
    save_color_state();
}
