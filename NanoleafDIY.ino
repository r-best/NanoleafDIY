#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "actions.h"
#include "constants.h"


void setup() {
    Serial.begin(9600);
    while(!Serial){;}
    port2.begin(9600);
    port3.begin(9600);

    leds.begin();

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
 * Reads the message from the given port.
 * If the message is a valid instruction, performs
 * the corresponding action
 */
void processMessage(uint8_t port){
    char *cmd = readSerial(port, -1);

    const char *token = strtok(cmd, " ");

    if(strcmp(token, "version") == 0)           PORTS[port]->println(get_version());
    else if(strcmp(token, "discover") == 0){
        char* temp = discover_network(port);
        PORTS[port]->println(temp);
        free(temp);
    }
    else if(strcmp(token, "set_color") == 0)    PORTS[port]->println(set_color());
    else                                        PORTS[port]->println(ERR_INVALID_COMMAND);

    free(cmd);
}
