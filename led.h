/**
 * This class controls the led strip
 */
#ifndef LED_H
#define LED_H

#include <stdint.h>

uint8_t active_colors[3] = {0, 0, 0};

int set_solid(uint8_t colors[3]){
    return 0;
}

#endif
