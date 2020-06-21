#include <time.h>
#include <string.h>
#include <inttypes.h>

#include "led.h"
#include "constants.h"


char* _get_next_arg(){
    return strtok(NULL, " ");
}

const char* get_version(){
    return VERSION;
}

/**
 * 
 */
boolean _request_discover(uint8_t port){
    if(PORTS[port] != &Serial) ((SoftwareSerial*)PORTS[port])->listen();
    PORTS[port]->print("discover");
    time_t start, now;
    time(&start);
    do {
        char* resp;
        int i = 0;
        while(PORTS[port]->available() > 0){
            resp[i] = PORTS[port]->read();
            i++;
        }
        if(strcmp(resp, "Acknowledged!") == 0)
            return true;
        time(&now);
    } while(difftime(now, start) < DISCOVERY_HANDSHAKE_TIMEOUT);
    return false;
}

/**
 * 
 */
void _receive_discover(uint8_t port, char** tree){
    if(PORTS[port] != &Serial) ((SoftwareSerial*)PORTS[port])->listen();
    time_t start, now;
    time(&start);
    do {
        int i = 0;
        while(PORTS[port]->available() > 0){
            *tree[i] = PORTS[port]->read();
            i++;
        }
        time(&now);
    } while(difftime(now, start) < DISCOVERY_RESPONSE_TIMEOUT);
}

/**
 * Recursive network discovery; the main node
 * will send this request to its neighbor, which
 * will check with both of its neighbors, etc..,
 * until the network is fully mapped
 */
const char* discover_network(uint8_t port){
    // Let requesting panel know we heard
    PORTS[port]->print("Acknowledged!");

    uint8_t left, right;
    if      (port == 0) { left = 1; right = 2; }
    else if (port == 1) { left = 2; right = 0; }
    else if (port == 2) { left = 0; right = 1; }

    // Ask left port to discover, see if it replies
    boolean left_active = _request_discover(left);
    
    // Ask right port to discover, see if it replies
    boolean right_active = _request_discover(right);

    char* left_tree;
    if(left_active) _receive_discover(left, &left_tree);

    char* right_tree;
    if(right_tree) _receive_discover(right, &right_tree);

    // Construct tree string
    
    // Set color to green
}

const char* set_color(){
    uint8_t colors[3] = {0, 0, 0};
    for(int i = 0; i < 3; i++){
        char *temp = _get_next_arg();
        if(temp == NULL) return ERR_INVALID_ARGS;

        int color = atoi(temp);
        if(color < 0 || color > 255) return ERR_INVALID_ARGS;

        colors[i] = color;
    }
    // set_solid(colors);
    return SUCCESS;
}
