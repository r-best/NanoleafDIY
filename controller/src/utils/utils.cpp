#include "utils.h"


SoftwareSerial *Log::debugSerial = new SoftwareSerial(debugLogRX, debugLogTX);

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