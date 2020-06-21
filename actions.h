#include <string.h>
#include <inttypes.h>

#include "led.h"
#include "utils.h"
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
    PORTS[port]->println("discover");
    char *resp = readSerial(port, DISCOVERY_HANDSHAKE_TIMEOUT);
    if(strcmp(resp, "Acknowledged!") == 0){
        free(resp);
        return true;
    }
    free(resp);
    return false;
}

/**
 * 
 */
void _receive_discover(uint8_t port, char** tree){
    if(PORTS[port] != &Serial) ((SoftwareSerial*)PORTS[port])->listen();
    char *resp = readSerial(port, DISCOVERY_RESPONSE_TIMEOUT);
}

/**
 * Recursive network discovery; the main node
 * will send this request to its neighbor, which
 * will check with both of its neighbors, etc..,
 * until the network is fully mapped
 */
const char* discover_network(uint8_t port){
    // Let requesting panel know we heard
    PORTS[port]->println("Acknowledged!");

    uint8_t left, right;
    if      (port == 0) { left = 1; right = 2; }
    else if (port == 1) { left = 2; right = 0; }
    else if (port == 2) { left = 0; right = 1; }

    // Ask left port to discover, see if it replies
    boolean left_active = _request_discover(left);
    Serial.println(left_active);

    // Ask right port to discover, see if it replies
    // boolean right_active = _request_discover(right);
    // Serial.println(right_active);

    char* left_tree;
    // if(left_active) _receive_discover(left, &left_tree);

    char* right_tree;
    // if(right_tree) _receive_discover(right, &right_tree);

    // Construct tree string
    // char* tree;
    // sprintf(tree, "(%s%s)",
    //     left_active ? left_tree : "X",
    //     right_active ? right_tree : "X"
    // );
    // PORTS[port]->println(tree);
    
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
