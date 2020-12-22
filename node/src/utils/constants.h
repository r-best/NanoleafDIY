/**
 * This class defines constant values to be used elsewhere in the program
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H


// Software version
#define VERSION "v0.1"

// Number of LEDs in the panel
#define NUM_LEDS 5
// Arduino pin connected to LED strip data line
#define LED_DATA_PIN 7
// Pin used to control pattern synchronization across panel network
#define INTERRUPT_PIN 2

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
#define MSG_SUCCESS "MSG_SUCCESS"
#define ERR_INVALID_COMMAND "Invalid command"
#define ERR_INVALID_ARGS "Command arguments were invalid"

#endif
