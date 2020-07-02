#include <string.h>
#include <stdlib.h>

#include <SoftwareSerial.h>

#include "tree.h"
#include "utils.h"
#include "constants.h"


void setup() {
    Serial.begin(115200);
    while(!Serial){;}

    Serial.println("Ready!");

    // Wait one second to make sure all connected panels come online
    delay(1000);

    // Send discovery command to connected panel in
    // a loop until it responds "Acknowledged!"
    while(true){
        // Send discovery command
        Serial.println("discover");

        // Read characters until we get a full message and check
        // if it's the acknowledgement
        char *resp = readSerial(-1);
        if(strcmp(resp, "Acknowledged!") != 0){
            free(resp);
            break;
        }
        free(resp);
    }

    // After discovery acknowledgement, wait for response with full network tree
    char *tree = readSerial(DISCOVERY_RESPONSE_TIMEOUT);
    Node* root = parse_tree(tree);
}

/**
 * Main control loop; 
 */
void loop() {
    
}
