#ifndef UTILS_H
#define UTILS_H

#include "constants.h"


// Web server object
ESP8266WebServer server(80);
void send_response(int code, const char* msg);

// Logging utilities
const boolean DEBUG = true;
SoftwareSerial debugSerial(5,4); // Corresponds to D1 and D2, NodeMCU has some weird ass pin numbering
struct {
    void begin()                        { if(DEBUG) debugSerial.begin(9600);    }
    void print(const char* msg)         { if(DEBUG) debugSerial.print(msg);     }
    void println(void)                  { if(DEBUG) debugSerial.println();      }
    void println(const char* msg)       { if(DEBUG) debugSerial.println(msg);   }
    void println(const Printable& x)    { if(DEBUG) debugSerial.print(x);       }
} Log;

/**
 * Reads a line from the given serial port, up until either the first "\n"
 * or until the optional timeout has passed
 * `timeout` can be set to -1 to run until first "\n" with no bound
 * Adds a string terminator to the input, since serial monitors usually
 * don't send those
 * Returns a malloc'd char pointer, don't forget to free it when you're done
 */
char* readSerial(int timeout) {
    char *line = (char*)malloc(sizeof(char)*COMMAND_BUFFER_SIZE);
    uint32_t command_buf_mult = 1;
    unsigned long start, now;
    start = millis();
    int i = 0;
    while(true) {
        // Read a character from the serial, if one is available
        if(Serial.available() > 0){
            char rc = Serial.read();

            // Once we hit a newline, we're done
            if (rc == '\n')
                break;

            line[i++] = rc;

            // Expand the buffer if we're running out of room
            if(i >= COMMAND_BUFFER_SIZE)
                line = (char*)realloc(line, sizeof(char)*COMMAND_BUFFER_SIZE*(++command_buf_mult));
        }
        
        // If we're out of time, quit the read and return NULL
        now = millis();
        if(timeout > 0 && now - start >= timeout){
            free(line);
            return NULL;
        }
    }

    // If it was a stupid Windows line return,
    // let the string terminator overwrite it
    if(line[i-1] == '\r') i--;

    // Terminate the string
    line[i] = '\0';

    return line;
}

#endif
