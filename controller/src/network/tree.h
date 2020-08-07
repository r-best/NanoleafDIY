/**
 * UNUSED LOGIC FOR PARSING THE TREE STRUCTURE FROM THE tree_encoding
 * Replicated this logic in the app instead, it's needed there to display
 * the panel configuration on the UI
 * I'm keeping this logic here for now in case I need it, but it's better
 * to just send the encoded string to the app instead
 * 
 * Phones have more memory than they could hope to use anyway
 */
#ifndef TREE_H
#define TREE_H

#include <Arduino.h>

#include "../utils/utils.h"


typedef struct Node {
    Node* parent;
    Node* left;
    Node* right;
    char* directions;   // Same representation as in forwarding command, a string of 'L's and 'R's
    int mode;       // Index of currently active lighting mode
    char* mode_data;    // String of configuration data for the current mode (e.g. for custom gradient mode, it would be a set of RGB values and transition times)
} Node;

extern char* tree_encoding; // Current state of the network
extern Node *root;

/**
 * Recursively discovers the topology of the panel network
 * --------------------------------------------------------------
 * Sends a 'discover' command to the first panel, which, if connected,
 * replies immediately with "Acknowledged!". That panel then sends the same
 * discover command to both of its neighbors, and waits to see if they
 * acknowledge, etc..
 * When this recursion reaches a leaf (neither neighbor acknowledges), the leaf replies
 * that it has no neighbors ("(XX)"). Its parent then combines the replies of both its children
 * and sends it back up to its parent (e.g. "((XX)X)" if the node has a leaf to the left and nothing to the right), etc..
 * This continues back up the tree until the first panel hands the completed string back to the controller
 * 
 * The resulting string encoding of the network is sent to the app to be rendered to the user
 */
const char* discover_network();

/**
 * Recursive funtion to free up all the malloc'd memory
 * used by the network tree, so the tree can be recalculated
 * Should be called with the root node as the argument
 */
void clear_tree();

const char* request_panel_state(char* directions);

#endif
