#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SoftwareSerial.h>

const char* VERSION = "v0.1";

// Common data structure that allows me to treat
// Serial and SoftwareSerial the same way
typedef struct {
    void (*begin)(unsigned long);
    boolean (*listen)();
    int (*available)();
    int (*read)();
    int (*print)(const char*);
} Port;

// Establish ports (first is Serial, other two
// are SoftwareSerials on pins 2/3 and 4/5)
SoftwareSerial port2(2, 3), port3(4, 5);
Stream* PORTS[3] = {
    &Serial,
    &port2,
    &port3
};


// Number of seconds the network discovery algorithm will
// wait for a neighbor to acknowledge the request
int DISCOVERY_HANDSHAKE_TIMEOUT = 1;
int DISCOVERY_RESPONSE_TIMEOUT = 10;

uint8_t COMMAND_BUFFER_SIZE = 250;
uint32_t command_buf_mult = 1;

// Response message constants
const char* SUCCESS = "Success";
const char* ERR_INVALID_COMMAND = "Invalid command";
const char* ERR_INVALID_ARGS = "Command arguments were invalid";

#endif
