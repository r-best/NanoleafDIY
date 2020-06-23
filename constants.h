/**
 * This class defines constant values to be used elsewhere in the program
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H


// Software version
const char* VERSION = "v0.1";

// Number of LEDs in the panel
uint8_t NUM_LEDS = 5;
// Arduino pin connected to LED strip data line
byte LED_DATA_PIN = 6;

// Number of milliseconds the network discovery algorithm will
// wait for a neighbor to acknowledge the request
int DISCOVERY_HANDSHAKE_TIMEOUT = 1000;
// Number of milliseconds the network discovery algorithm will
// wait for a neighbor to return the full response, after acknowledgement
int DISCOVERY_RESPONSE_TIMEOUT = 10000;

// Initial size of the buffer used to read serial commands, gets realloc'd
// by this amount if it runs out of space
uint8_t COMMAND_BUFFER_SIZE = 250;

// Response message constants
const char* SUCCESS = "Success";
const char* ERR_INVALID_COMMAND = "Invalid command";
const char* ERR_INVALID_ARGS = "Command arguments were invalid";

#endif
