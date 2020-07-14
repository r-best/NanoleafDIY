/**
 * This file contains the implementations of the actions the panel can take
 * upon receiving different commands, as defined in the main file
 * 
 * Commands:
 *  - `fwd`: Forwards another command to a panel further down the tree
 *  - `discover`: Maps the tree structure of all panels to the left and right, returning it to the caller
 *  - `get_version`: Returns the version of the software
 *  - `set_color`: Sets all the LEDs to the given RGB value
 */
#include <string.h>
#include <inttypes.h>

#include "led.h"
#include "utils.h"
#include "constants.h"


/**
 * This command returns the version of the software running on the device
 */
const char* get_version(){
    return VERSION;
}

/**
 * This command is used to send other commands to specific nodes in the tree by
 * forwarding them along a path through the connecting nodes
 * Command format: `0<directions>|<command>`
 *  0 is the forward command, it's dropped when the command is passed into this function,
 *      so the `data` string starts with the first direction
 *  <directions> is a string of L's and R's, dictating which direction to turn
 *      at each node in the tree. When a node receives this command it pops the
 *      first character from the string, and forwards the request in that direction
 *  <command> is the command to be executed by the target node, can be any other valid command
 */
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
 * 
 */
boolean _request_discover(uint8_t port){
    // Send discovery command to given port
    if(PORTS[port] != &Serial) ((SoftwareSerial*)PORTS[port])->listen();
    PORTS[port]->println("1");

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
 * 
 */
void _receive_discover(uint8_t port, char** tree){
    if(PORTS[port] != &Serial) ((SoftwareSerial*)PORTS[port])->listen();
    *tree = readSerial(port, DISCOVERY_RESPONSE_TIMEOUT);
}

/**
 * Recursive Network Discovery ---------------------------------------------------------------
 * When a node receives this command it immediately replies "Acknowledged!" to let the caller
 * know they're present, and then sends the same command to its left and right neighbors. When
 * it receives their full responses, it concatenates them, surrounds them with parenthesis, and
 * sends them back to the caller (a neighbor that doesn't reply is replaced with an 'X').
 * 
 * When the master unit sends this command to its first node, it effectively maps the entire network
 * 
 * Example:
 *            O
 *           /             
 *   M*O -- O -- O    ------->      (((XX)(XX))(XX))
 *     |
 *     O
 */
char* discover_network(uint8_t port){
    // Let requesting panel know we heard
    PORTS[port]->println("Acknowledged!");

    // Get left and right port relative to request origin
    uint8_t left, right;
    if      (port == 0) { left = 1; right = 2; }
    else if (port == 1) { left = 2; right = 0; }
    else if (port == 2) { left = 0; right = 1; }

    // Ask left port to start discovering, see if it replies
    boolean left_active = _request_discover(left);

    // Ask right port to start discovering, see if it replies
    boolean right_active = _request_discover(right);

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
    
    // Set color to green to show discovery has completed
    set_solid(0, 10, 0);

    return tree;
}

/**
 * Sets all of the LEDs to the given color
 * Command format: `2#<hex value>`
 *  Where 'hex value' is the desired hexadecimal color code
 */
const char* set_color(char* data){
    uint8_t colors[3] = {0, 0, 0};
    for(int i = 0; i < 3; i++){
        char temp[3] = { data[i*2], data[i*2+1], '\0' };
        colors[i] = strtol(temp, NULL, 16);

        if(colors[i] < 0 || colors[i] > 255)
            return ERR_INVALID_ARGS;
    }
    set_solid(colors[0], colors[1], colors[2]);
    return MSG_SUCCESS;
}

/**
 * Sets the LED strip to the given predefined pattern
 * Command format: `3<pattern>`
 *  Where pattern is a number corresponding to a predefined pattern
 */
const char* set_pattern(char* data){
    int pattern = atoi(data);
    if(pattern < 0 || pattern > 255) return ERR_INVALID_ARGS;

    set_mode(pattern);
    return MSG_SUCCESS;
}

/**
 * Sets the ms delay used in updating the LED pattern
 * Command format: `4<speed>`
 *  Where speed is the number of ms to delay LED pattern updates
 */
const char* set_speed(char* data){
    int speed = atoi(data);
    if(speed < 0 || speed > 255) return ERR_INVALID_ARGS;

    set_refresh_rate(speed);
    return MSG_SUCCESS;
}
