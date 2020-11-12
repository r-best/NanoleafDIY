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


/**
 * Linked list binary tree structure that tracks the arrangement of the conneted panels
 * Updated every time `discover_network()` is called (i.e. on startup and every time a user hits GET /network/refresh)
 * 
 * Fields:
 *  - Nodes (parent, left, right): Pointers to respective neighbors
 *  - directions: The panel's position relative to the controller, represented as a string of 'L's and 'R's (same format as in forwarding command)
 *      (i.e. the panel connected to the controller is "", its left neighbor is "L", that one's right neighbor is "LR", etc..)
 *  - mode: Integer index of the currently active mode on the panel, detailed in the panel code documentation
 *      (e.x. 0 is solid color mode, 1 is custom gradient mode, 2 is rainbow, etc..)
 *  - mode_data: Custom data used by certain light modes
 *      (e.x. An RGB color value for solid color mode, an array of RGB values/transition times for custom gradient mode)
 * 
 *  *Note: The destructor acts recursively to destroy all the Node's children
 */
typedef struct Node {
    Node* parent;
    Node* left;
    Node* right;
    char* directions;   // Same representation as in forwarding command, a string of 'L's and 'R's
    int mode;           // Index of currently active lighting mode
    bool randomize, synchronize;
    long length;
    char* mode_data;    // String of configuration data for the current mode (e.g. for custom gradient mode, it would be a set of RGB values and transition times)

    Node(Node* parent, uint8_t directions_length){
        this->directions = (char*)malloc(directions_length+1);
        this->parent = parent;
        this->left = NULL;
        this->right = NULL;
        this->mode = -1;
        this->mode_data = NULL;
    }
    ~Node(){
        free(this->directions);
        if(this->mode_data != NULL) free(this->mode_data);

        if(this->left != NULL) delete this->left;
        if(this->right != NULL) delete this->right;
    }
} Node;

extern char* tree_encoding; // Current state of the network
extern Node *root;

Node* fetch_panel(const char* directions);

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

#endif
