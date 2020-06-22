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
    *tree = readSerial(port, DISCOVERY_RESPONSE_TIMEOUT);
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

    // Ask right port to discover, see if it replies
    boolean right_active = _request_discover(right);

    char* left_tree;
    if(left_active) _receive_discover(left, &left_tree);
    else { left_tree = (char*)malloc(2); strcpy(left_tree, "X"); }

    char* right_tree;
    if(right_active) _receive_discover(right, &right_tree);
    else { right_tree = (char*)malloc(2); strcpy(right_tree, "X"); }

    // Construct first half of tree string by adding parenthesis to start of left_tree
    // Could technically be done all at once but I'm concerned about the memory footprint
    char* tree = (char*)malloc(strlen(left_tree) + 1);
    strcpy(tree, "(");
    strcat(tree, left_tree);
    free(left_tree);

    // Construct second half of tree string, now that we've freed up memory
    tree = (char*)realloc(tree, strlen(tree) + strlen(right_tree)); // +0 because we're adding a parenthesis and losing a \0
    strcat(tree, right_tree);
    strcat(tree, ")");
    free(right_tree);
    
    // Set color to green
    // set_solid(0, 255, 0);

    return tree;
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
