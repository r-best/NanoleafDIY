/**
 * This class defines constant values to be used elsewhere in the program
 */
#ifndef PROJ_CONSTANTS_H
#define PROJ_CONSTANTS_H


#define DEBUG true

// Software version
#define VERSION "v0.1"

// Serial pins used for debug logging
// Corresponds to D1 and D2, NodeMCU has some weird ass pin numbering
#define debugLogRX 5
#define debugLogTX 4

// Number of milliseconds the network discovery algorithm will
// wait for a neighbor to acknowledge the request
#define DISCOVERY_HANDSHAKE_TIMEOUT 1000
// Number of milliseconds the network discovery algorithm will
// wait for a neighbor to return the full response, after acknowledgement
#define DISCOVERY_RESPONSE_TIMEOUT 10000

// Initial size of the buffer used to read serial commands, gets realloc'd
// by this amount if it runs out of space
#define COMMAND_BUFFER_SIZE 250

// Response message constants
#define MSG_SUCCESS                 "MSG_SUCCESS"
#define ERR_PARSE_REQ_BODY          "Error deserializing JSON request"
#define ERR_NO_PANELS_CONNECTED     "No panels are connected to the controller"
#define ERR_READ_TIMEOUT            "Read timed out"
#define ERR_NON_ACKNOWLEDGEMENT     "Non-acknowledgement received for network discovery"
#define ERR_PANEL_STATE_RESPONSE    "Error fetching panel state"
#define ERR_TREE_PARSE              "Error parsing tree encoding"

#endif
