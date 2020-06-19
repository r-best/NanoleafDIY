#include <string>
#include <unordered_map>

#include "led.c"
#include "constants.h"
using namespace std;

typedef char* (*func)(void);
unordered_map<string, func> ACTIONS = {
    { "version", [](){ return VERSION; } },
    { "discover", &discover_network },
    { "set_color", &set_color }
};

char* _get_next_arg(){
    return strtok(NULL, " ");
}

char* discover_network(){
    return "Not implemented";
}

char* set_color(){
    uint8_t colors[3] = {0, 0, 0};
    for(int i = 0; i < 3; i++){
        char *temp = _get_next_arg();
        if(temp == NULL) return ERR_INVALID_ARGS;

        int color = atoi(temp);
        if(color < 0 || color > 255) return ERR_INVALID_ARGS;

        colors[i] = color;
    }
    set_solid(colors);
    return SUCCESS;
}
