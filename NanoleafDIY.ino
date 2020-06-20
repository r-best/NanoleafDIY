#include <string.h>

#include "actions.h"
#include "constants.h"


void setup() {
    Serial.begin(9600);
    while(!Serial){;}
    port2.begin(9600);
    port3.begin(9600);
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
    char *cmd;
    int i = 0;
    while(PORTS[port].available() > 0){
        cmd[i] = PORTS[port].read();
        i++;
    }

    const char *token = strtok(cmd, " ");

    if(strcmp(token, "version") == 0) PORTS[port].print(get_version());
    else if(strcmp(token, "discover") == 0) PORTS[port].print(discover_network(port));
    else if(strcmp(token, "set_color") == 0) PORTS[port].print(set_color(port));
    else PORTS[port].print(ERR_INVALID_ARGS);
}
