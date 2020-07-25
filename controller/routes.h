#include <string.h>
#include <stdlib.h>

#include "utils.h"


/**
 * Recursively discovers the topology of the panel network
 * --------------------------------------------------------------
 * Sends a 'discover' command to the first panel, which, if connected,
 * replies immediately with "Acknowledged!". That panel then sends the same
 * discover command to both of its neighbors, and waits to see if they
 * acknowledge, etc..
 * When this recursion reaches a leaf (neither neighbor acknowledges), the leaf replies
 * that it has no neighbors ("(XX)"). Its parent then combines the replies of both its children
 * and sends it back up to its parent ("((XX)X)" if the node has a leaf to the left and nothing to the right), etc..
 * This continues back up the tree until the first panel hands the completed string back to the controller
 */
void discover_network(){
    Log.println("Incoming request: Network Discovery");

    // Send discovery command
    Serial.println("1");

    // Wait to see if acknowledgement arrives
    char *resp = readSerial(DISCOVERY_HANDSHAKE_TIMEOUT);
    if(resp == NULL){
        Log.println("Network discovery acknowledgement timeout; no panels connected");
        send_response(204, "No panels connected"); // 204 no content :shrug:
        free(resp);
        return;
    }
    if(strcmp(resp, "Acknowledged!") != 0){
        Log.print(ERR_NON_ACKNOWLEDGEMENT);Log.print(": ");Log.println(resp);
        send_response(500, ERR_NON_ACKNOWLEDGEMENT);
        free(resp);
        return;
    }
    free(resp);

    // After discovery acknowledgement, wait for response with full network tree
    char* tree = readSerial(DISCOVERY_RESPONSE_TIMEOUT);
    if(tree == NULL){
        Log.println(ERR_READ_TIMEOUT);
        send_response(500, "Read timeout in network discovery");
        free(tree);
        return;
    }

    send_response(200, tree);
    free(tree);
}
// Mock network discovery for when I don't have another panel plugged in
char* discover_network_DEBUG(){
    const char* input = "(((XX)X)(X((XX)X)))";
    char* tree = (char*)malloc(strlen(input));
    strcpy(tree, input);
    return tree;
}
