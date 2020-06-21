#include <string.h>
#include <stdlib.h>

#include "actions.h"
#include "constants.h"


void setup() {
    Serial.begin(9600);
    while(!Serial){;}
    port2.begin(9600);
    port3.begin(9600);

    Serial.println("Ready!");
}

/**
 * Main control loop; checks each port for messages
 * in sequence, acting on any it finds
 */
void loop() {
    if(Serial.available() > 0) processMessage(0);

    port2.listen();
    if(port2.available() > 0) processMessage(1);

    port3.listen();
    if(port3.available() > 0) processMessage(2);
}

/**
 * Reads a command from the given port byte
 * by byte until a line return is reached
 */
void readSerial(uint8_t port, char** cmd) {
    int i = 0;
    while (true) {
        if(Serial.available() > 0){
            char rc = PORTS[port]->read();
            if (rc == '\n')
                break;

            (*cmd)[i++] = rc;
        }
        // Expand the buffer if we're running out of room
        if(i >= COMMAND_BUFFER_SIZE)
            *cmd = (char*)realloc(*cmd, sizeof(char)*COMMAND_BUFFER_SIZE*(++command_buf_mult));
    }
    // If it was a stupid Windows line return,
    // let the string terminator overwrite it
    if((*cmd)[i-1] == '\r') i--;

    // Terminate the string
    (*cmd)[i] = '\0';
}

/**
 * Reads the message from the given port.
 * If the message is a valid instruction, performs
 * the corresponding action
 */
void processMessage(uint8_t port){
    char *cmd = (char*)malloc(sizeof(char)*COMMAND_BUFFER_SIZE);
    readSerial(port, &cmd);

    const char *token = strtok(cmd, " ");

    if(strcmp(token, "version") == 0)           PORTS[port]->println(get_version());
    else if(strcmp(token, "discover") == 0)     PORTS[port]->println(discover_network(port));
    else if(strcmp(token, "set_color") == 0)    PORTS[port]->println(set_color());
    else                                        PORTS[port]->println(ERR_INVALID_COMMAND);

    free(cmd);
}
