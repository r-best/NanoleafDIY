#include "leds.h"

#include "../state/state.h"


Adafruit_NeoPixel leds(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

// Index of the currently selected builtin pattern
uint8_t current_mode = 0;
unsigned long timer;

/** List of color modes, implementations in `led_patterns.h` */
Pattern *MODES[] = {
    NULL, // Space for solid color pattern, loaded from EEPROM in `setup_leds()`
    NULL, // Space for custom gradient pattern, loaded from EEPROM in `setup_leds()`
    NULL, // Space for blinking pattern, loaded from EEPROM in `setup_leds()`
    new RainbowPattern(),
    new TheaterChase(),
    new TheaterChaseRainbow()
};

void setup_leds(){
    leds.begin();
    leds.setBrightness(25);

    // Load lighting state from EEPROM
    MODES[0] = get_solid_color_state();
    MODES[1] = get_custom_gradient_state();
    MODES[2] = get_blink_state();
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

void set_refresh_rate(uint8_t ms){ MODES[current_mode]->refresh_rate = ms; }

int set_mode(uint8_t mode){
    MODES[mode]->init();
    current_mode = mode;
    save_current_mode_state();
    return 0;
}

/** Updates the solid color setting */
void set_solid(uint8_t r, uint8_t g, uint8_t b){
    delete MODES[0];
    MODES[0] = new SolidColor(r, g, b);
    MODES[0]->init();
    save_solid_color_state();
}

void set_custom_gradient(uint8_t length, uint8_t *r, uint8_t *g, uint8_t *b, uint32_t *transitions){
    delete MODES[1];
    MODES[1] = new FadingGradient(length, r, g, b, transitions);
    MODES[1]->init();
    save_custom_gradient_state();
}


void set_blink(uint8_t length, uint8_t *r, uint8_t *g, uint8_t *b, uint32_t *times){
    delete MODES[2];
    MODES[2] = new Blink(length, r, g, b, times);
    MODES[2]->init();
    save_blink_state();
}
