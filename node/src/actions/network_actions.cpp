#include "actions.h"

#include <Arduino.h>

#include "../leds/led_patterns.h"
#include "../utils/utils.h"


// Define the panel's ports (initialized in the main file)
extern Stream* PORTS[3];

const char* forward_cmd(uint8_t port, char* data){
    // Get left and right port relative to request origin
    uint8_t left, right;
    if      (port == 0) { left = 1; right = 2; }
    else if (port == 1) { left = 2; right = 0; }
    else if (port == 2) { left = 0; right = 1; }

    // Reads the first direction in the list to decide where to forward the message ('L' or 'R')
    uint8_t target_port;
    if(data[0] == 'L')  target_port = left;
    else                target_port = right;

    // If this is the last direction, we'll send the actual command instead of another forward
    if(data[1] == '|'){
        PORTS[target_port]->println(data+2);
    } else {
        data[0] = '0'; // Replace the used direction with the forward command for the next panel to read
        PORTS[target_port]->println(data);
    }

    return MSG_SUCCESS;
}

/**
 * Helper method for network discovery, sends a discovery request to
 * the given neighbor and returns true or false if they acknowledge
 * 
 * Used in conjunction with `_receive_discover()`, which should be called
 * after this to wait for the full network response
 */
static bool _request_discover(uint8_t port){
    // Send discovery command to given port
    if(PORTS[port] != &Serial) ((SoftwareSerial*)PORTS[port])->listen();
    PORTS[port]->println("2");

    // Wait for acknowledgement
    char *resp = readSerial(port, DISCOVERY_HANDSHAKE_TIMEOUT);
    if(strcmp(resp, "Acknowledged!") == 0){
        free(resp);
        return true;
    }
    free(resp);
    return false;
}

/**
 * Helper method for network discovery, should be called after `_request_discover()`
 * if it returned true (i.e. the neighbor acknowledged the request)
 * 
 * Waits for the response containing the full network tree encoding
 */
static void _receive_discover(uint8_t port, char** tree){
    if(PORTS[port] != &Serial) ((SoftwareSerial*)PORTS[port])->listen();
    *tree = readSerial(port, DISCOVERY_RESPONSE_TIMEOUT);
}

char* discover_network(uint8_t port){
    // Let requesting panel know we heard
    PORTS[port]->println("Acknowledged!");

    // Get left and right port relative to request origin
    uint8_t left, right;
    if      (port == 0) { left = 1; right = 2; }
    else if (port == 1) { left = 2; right = 0; }
    else if (port == 2) { left = 0; right = 1; }

    // Ask left port to start discovering, see if it replies
    bool left_active = _request_discover(left);

    // Ask right port to start discovering, see if it replies
    bool right_active = _request_discover(right);

    // If left port replied, wait for its full response ('X' if it didn't)
    char* left_tree;
    if(left_active) _receive_discover(left, &left_tree);
    else { left_tree = (char*)malloc(2); strcpy(left_tree, "X"); }

    // If right port replied, wait for its full response ('X' if it didn't)
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

    return tree;
}

void fetch_state_action(uint8_t port, char* directions){
    // Get left and right port relative to request origin
    uint8_t left, right;
    if      (port == 0) { left = 1; right = 2; }
    else if (port == 1) { left = 2; right = 0; }
    else if (port == 2) { left = 0; right = 1; }

    // Reads the first direction in the list to decide where to forward the message ('L' or 'R')
    uint8_t target_port;
    if      (directions[0] == 'L') target_port = left;
    else if (directions[0] == 'R') target_port = right;
    else { // Else there are no directions left, so this panel is the target
        if(current_mode == 0 || current_mode == 1){ // If mode is 0 or 1, they use the color set so append the color set data
            char *resp = (char*)malloc(10*palette->length + 3);
            sprintf(resp, "%d%d", current_mode, palette->length);
            for(int i = 0; i < palette->length; i++)
                sprintf(resp + i*10 + 2, "%02X%02X%02X%04d", (*palette)[i].r, (*palette)[i].g, (*palette)[i].b, (*palette)[i].time);
            PORTS[port]->println(resp);
            free(resp);
        } else { // All other modes have no configurable settings, so we can just send back the mode number
            char resp[2];
            sprintf(resp, "%d", current_mode);
            PORTS[port]->println(resp);
        }
        return;
    }

    // If the direction was 'L' or 'R', forward the request in that direction and wait for a response
    directions[0] = '3'; // Replace the used direction with the command for the next panel to read
    PORTS[target_port]->println(directions);

    // Read response and return to caller
    char *resp;
    if((resp = readSerial(target_port, DISCOVERY_RESPONSE_TIMEOUT)) == NULL)
        PORTS[port]->println("Error fetching panel state");
    else
        PORTS[port]->println(resp);
    free(resp);
}
