#include "actions.h"


void set_solid_color_action(char* data){
    uint8_t colors[3] = {0, 0, 0};
    for(int i = 0; i < 3; i++){
        if(data[0] == '#'){ // If hexcode
            char temp[3] = { data[(i*2)+1], data[i*2+2], '\0' };
            colors[i] = strtol(temp, NULL, 16);
        } else { // If rgb
            char temp[4] = { data[i*3], data[i*3+1], data[i*3+2], '\0' };
            colors[i] = strtol(temp, NULL, 10);
        }

        if(colors[i] < 0 || colors[i] > 255)
            return;
    }

    set_solid(colors[0], colors[1], colors[2]);
}

void set_mode_action(char* data){
    int pattern = atoi(data);
    if(pattern < 0 || pattern > 255)
        return;

    set_mode(pattern);
}

void set_refresh_rate_action(char* data){
    int speed = atoi(data);
    if(speed < 0 || speed > 255)
        return;

    set_refresh_rate(speed);
}

void set_gradient_action(char* data){
    uint8_t length = data[0] - '0';
    uint8_t *r = (uint8_t*)malloc(sizeof(uint8_t)*length);
    uint8_t *g = (uint8_t*)malloc(sizeof(uint8_t)*length);
    uint8_t *b = (uint8_t*)malloc(sizeof(uint8_t)*length);
    uint32_t *transitions = (uint32_t*)malloc(sizeof(uint32_t)*length);
    for(int i = 0; i < length; i++){
        char tempR[] = { data[(i*10)+1], data[(i*10)+2], '\0' };
        r[i] = (uint8_t)strtol(tempR, NULL, 16);

        char tempG[] = { data[(i*10)+3], data[(i*10)+4], '\0' };
        g[i] = (uint8_t)strtol(tempG, NULL, 16);

        char tempB[] = { data[(i*10)+5], data[(i*10)+6], '\0' };
        b[i] = (uint8_t)strtol(tempB, NULL, 16);

        char tempTransitions[] = { data[(i*10)+7], data[(i*10)+8], data[(i*10)+9], data[(i*10)+10], '\0' };
        transitions[i] = (uint32_t)strtol(tempTransitions, NULL, 10);
    }

    set_custom_gradient(length, r, g, b, transitions);
}
