#include <stdlib.h>

#include <SoftwareSerial.h>

#include "src/utils/utils.h"
#include "src/leds/leds.h"
#include "src/actions/actions.h"


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

    setup_leds();
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

    switch(cmd[0]){
        case '0': // Forwarding command
            forward_cmd(port, cmd+1); break;
        case '1': { // Network discovery command
            char* tree = discover_network(port);
            PORTS[port]->println(tree);
            free(tree);
            break;
        }
        case '2': // Set solid color command
           set_color(cmd+1); break;
        case '3': // Set pattern command
            set_pattern(cmd+1); break;
        case '4': // Set refresh rate command
            set_speed(cmd+1); break;
        case '5':
            set_custom_pattern(cmd+1); break;
        case '6': // Get version command
            PORTS[port]->println(VERSION); break;
        default:
            PORTS[port]->println(ERR_INVALID_COMMAND); break;
    }

    free(cmd);
}
