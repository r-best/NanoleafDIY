#include "tree.h"


char* tree_encoding = ""; // Current state of the network
Node* root = NULL;

/**
 * Helper method for `discover_network()`, called at the very end, after the panels have been
 * discovered & registered in the linked list
 * Iterates through the linked list depth-first and sends a request to each panel to obtain its
 * current lighting status, updating the linked list Node accordingly
 */
static int request_panel_states(){
    bool right = false;
    Node *current = root;
    while(true){
        Log::println(current->directions);
        if(current == root && current->mode > -1) break; // If we've come back up to the root after processing it, we've traversed entire tree

        if(current->mode == -1){ // Prevent processing same node twice
            // Construct status command & send it
            char *cmd = (char*)malloc(strlen(current->directions) + 2);
            sprintf(cmd, "2%s", current->directions);
            Log::print("Sending command: "); Log::println(cmd);
            Serial.println(cmd);
            free(cmd);

            // Wait for response
            char *resp = readSerial(DISCOVERY_RESPONSE_TIMEOUT);
            if(resp == NULL || strcmp(resp, ERR_PANEL_STATE_RESPONSE) == 0){
                free(resp);
                return -1;
            }

            // Record light status data
            current->mode = resp[0] - '0';
            switch(current->mode){
                case 0: case 1: // Lighting modes that have custom config data
                    current->mode_data = (char*)malloc(strlen(resp));
                    strcpy(current->mode_data, resp+1);
                    break;
                default: // Other modes don't need custom data
                    current->mode_data = NULL;
                    break;
            }
            Log::print("Panel |");Log::print(current->directions);Log::print("| - Mode ");Log::println(current->mode);
        }

        // Navigate to next unvisited node (preference for left, then right, then back up the tree)
        if      (current->left   != NULL && current->left->mode  == -1) current = current->left;
        else if (current->right  != NULL && current->right->mode == -1) current = current->right;
        else if (current->parent != NULL)                               current = current->parent;
    }
    return 0;
}

/**
 * Helper method for `discover_network()`
 * Parses the string encoding of the tree into a linked list representation
 * Each Node in the linked list contains references to its neighbors as well as the set of directions
 * required to reach it
 */
static int parse_tree(char* tree){
    // Clear the old tree out of memory, if there is one
    if(root != NULL){
        free(tree_encoding);
        delete root;
    }

    tree_encoding = tree;

    // Create first node
    root = new Node(NULL, 0);
    strcpy(root->directions, "");

    Node* active = root;
    boolean next_is_right = false;
    int active_directions_length = 1;
    Log::println(tree_encoding);
    for(int i = 1; i < strlen(tree_encoding)-1; i++){
        // On an open parenthesis, we add a new node to the tree
        if(tree_encoding[i] == '('){
            Node *child = new Node(active, active_directions_length++);
            strcpy(child->directions, active->directions);
            if(next_is_right){
                active->right = child;
                strcat(child->directions, "R");
                next_is_right = false;
            }
            else{
                active->left = child;
                strcat(child->directions, "L");
            }
            active = child;
            Log::println(child->directions);
        }
        // On a close parenthesis, we go back to the parent node and set
        // that the next node added will be on the right
        else if (tree_encoding[i] == ')'){
            active = active->parent;
            next_is_right = true;
        }
    }
    return 0;
}

const char* discover_network(){
    // Send discovery command
    Serial.println("1");

    // Wait to see if acknowledgement arrives
    char *resp = readSerial(DISCOVERY_HANDSHAKE_TIMEOUT);
    // If the read timed out, there are no panels connected, need to reflect that in linked list & tree encoding
    if(resp == NULL){
        Log::println("Network discovery acknowledgement timeout; no panels connected");
        free(resp);
        if(root != NULL){
            tree_encoding = (char*)realloc(tree_encoding, 1);
            tree_encoding[0] = '\0';
            delete root;
        }
        return tree_encoding;
    }
    // If the response wasn't an acknowledgement, something went wrong
    if(strcmp(resp, "Acknowledged!") != 0){
        Log::print(ERR_NON_ACKNOWLEDGEMENT);Log::print(": ");Log::println(resp);
        free(resp);
        return ERR_NON_ACKNOWLEDGEMENT;
    }
    free(resp);

    // After discovery acknowledgement, wait for response with full network tree
    char* tree = readSerial(DISCOVERY_RESPONSE_TIMEOUT);
    if(tree == NULL){
        Log::println(ERR_READ_TIMEOUT);
        free(tree);
        return ERR_READ_TIMEOUT;
    }

    // Parse the tree encoding and store the data in the linked list structure
    if(parse_tree(tree)){
        Log::println(ERR_TREE_PARSE);
        free(tree);
        return ERR_TREE_PARSE;
    }

    // Request the light settings of all panels
    if(request_panel_states())
        Log::println(ERR_PANEL_STATE_RESPONSE);

    return tree_encoding;
}

Node* fetch_panel(const char* directions){
    // Find correct Node in stored linked list network representation
    Node *current = root;
    for(int i = 0; i < strlen(directions); i++){
        if(directions[i] == 'L')
            current = current->left;
        else if(directions[i] == 'R')
            current = current->right;

        // If we just moved to a panel that doesn't exist, return an error
        if(current == NULL)
            return NULL;
    }
    return current;
}
