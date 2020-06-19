#include <string.h>
#include <SoftwareSerial.h>

#include "actions.h"
#include "constants.h"


// Establish ports (first is Serial, other two
// are SoftwareSerials on pins 2/3 and 4/5)
SoftwareSerial port2(port2RX, port2TX), port3(port3RX, port3TX);
Stream* PORTS[3] = {
    &Serial,
    &port2,
    &port3
};

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
    while(PORTS[port]->available() > 0){
        cmd[i] = PORTS[port]->read();
        i++;
    }

    char *token = strtok(cmd, " ");

    if(ACTIONS.find(cmd) != ACTIONS.end())
        PORTS[port]->println(ACTIONS[token]());
    else
        PORTS[port]->println("Invalid command name");
}
