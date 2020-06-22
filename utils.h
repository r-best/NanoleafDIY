#ifndef UTILS_H
#define UTILS_H


#include "constants.h"


/**
 * Reads a line from the given serial port, up until either the first "\n"
 * or until the optional timeout has passed
 * `timeout` can be set to -1 to run until first "\n" with no bound
 * Adds a string terminator to the input, since serial monitors usually
 * don't send those
 * Returns a malloc'd char pointer, don't forget to free it when you're done
 */
char* readSerial(uint8_t port, int timeout) {
    char *line = (char*)malloc(sizeof(char)*COMMAND_BUFFER_SIZE);
    unsigned long start, now;
    start = millis();
    int i = 0;
    do {
        if(Serial.available() > 0){
            char rc = PORTS[port]->read();
            if (rc == '\n')
                break;

            line[i++] = rc;

            // Expand the buffer if we're running out of room
            if(i >= COMMAND_BUFFER_SIZE)
                line = (char*)realloc(line, sizeof(char)*COMMAND_BUFFER_SIZE*(++command_buf_mult));
        }
        
        now = millis();
    } while(timeout <= 0 || now - start < timeout);
    // If it was a stupid Windows line return,
    // let the string terminator overwrite it
    if(line[i-1] == '\r') i--;

    // Terminate the string
    line[i] = '\0';

    return line;
}

#endif
