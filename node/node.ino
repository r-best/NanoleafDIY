#include <string.h>
#include <stdlib.h>

#include <SoftwareSerial.h>

#include "utils.h"
#include "actions.h"
#include "constants.h"


// TODO BUGFIX: That thing where it writes "TestingOpen" on startup and it gets accepted as an invalid command
// Causes the first network discovery request to fail

// Establish ports (first is Serial, other two
// are SoftwareSerials on pins 2/3 and 4/5)
SoftwareSerial port2(2, 3), port3(4, 5);
Stream* PORTS[3] = {
    &Serial,
    &port2,
    &port3
};

void setup() {
    Serial.begin(115200);
    while(!Serial){;}
    port2.begin(115200);
    port3.begin(115200);

    leds.begin();
    leds.setBrightness(25);
}

/**
 * Main control loop; checks each port for messages
 * in sequence, acting on any it finds
 */
void loop() {
    update_leds();

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

    if(strcmp(token, "discover") == 0){
        char* temp = discover_network(port);
        PORTS[port]->println(temp);
        free(temp);
    }
    else if(strcmp(token, "fwd") == 0)          PORTS[port]->println(forward_cmd(port, &cmd));
    else if(strcmp(token, "set_color") == 0)    PORTS[port]->println(set_color());
    else if(strcmp(token, "set_pattern") == 0)  PORTS[port]->println(set_pattern());
    else if(strcmp(token, "set_speed") == 0)    PORTS[port]->println(set_speed());
    else if(strcmp(token, "version") == 0)      PORTS[port]->println(get_version());
    else                                        PORTS[port]->println(ERR_INVALID_COMMAND);

    free(cmd);
}
