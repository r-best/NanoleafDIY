#include "actions.h"


void set_mode_action(char* data){
    int pattern = atoi(data);
    if(pattern < 0 || pattern > 255)
        return;

    set_mode(pattern);
}

void set_brightness_action(char* data){
    int brightness = atoi(data);
    if(brightness < 0 || brightness > 255)
        return;

    set_brightness(brightness);
}

/** This is literally the same as `set_blinking_action` except it calls the gradient function on the last line, sorry */
void set_color_state_action(char* data){
    bool randomize = data[0] - '0' == 1;
    bool synchronize = data[1] - '0' == 1;
    uint8_t length = data[2] - '0';
    ColorStep *steps = (ColorStep*)malloc(sizeof(ColorStep)*length);
    for(int i = 0; i < length; i++){
        char tempR[] = { data[(i*10)+3], data[(i*10)+4], '\0' };
        char tempG[] = { data[(i*10)+5], data[(i*10)+6], '\0' };
        char tempB[] = { data[(i*10)+7], data[(i*10)+8], '\0' };
        char tempTimes[] = { data[(i*10)+9], data[(i*10)+10], data[(i*10)+11], data[(i*10)+12], '\0' };

        steps[i] = ColorStep(
            (uint8_t)strtol(tempR, NULL, 16),
            (uint8_t)strtol(tempG, NULL, 16),
            (uint8_t)strtol(tempB, NULL, 16),
            (uint32_t)strtol(tempTimes, NULL, 10)
        );
    }

    set_color_state(steps, length, randomize, synchronize);
}
// 6012FFFFFF05000000000500
// 6013FF0000050000FF0005000000FF0500