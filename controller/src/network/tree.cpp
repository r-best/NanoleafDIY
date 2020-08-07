#include "tree.h"


char* tree_encoding = NULL; // Current state of the network
Node* root = NULL;

static void _clear_tree(Node* node){
    free(node->directions);
    if(node->mode_data != NULL) free(node->mode_data);

    if(node->left) _clear_tree(node->left);
    if(node->right) _clear_tree(node->right);

    free(node);
}
void clear_tree(){
    free(tree_encoding);
    _clear_tree(root);
}

/**
 * Parses the string encoding obtained from `discover_network()` into a linked list representation
 * Each Node in the linked list contains references to its neighbors as well as the set of directions
 * required to reach it
 */
static void parse_tree(char* tree){
    if(root != NULL) clear_tree();

    tree_encoding = tree;
    int input_length = strlen(tree_encoding);

    root = (Node*)malloc(sizeof(Node));
    root->directions = (char*)malloc(1);
    strcpy(root->directions, "");
    root->mode = -1;
    root->mode_data = NULL;
    root->parent = NULL;
    root->left = NULL;
    root->right = NULL;

    Node* active = root;
    boolean next_is_right = false;
    int active_directions_length = 1;
    for(int i = 1; i < input_length-1; i++){
        // On an open parenthesis, we add a new node to the tree
        if(tree_encoding[i] == '('){
            Node *child = (Node*)malloc(sizeof(Node));
            child->parent = active;
            child->directions = (char*)malloc(active_directions_length++);
            strcpy(child->directions, active->directions);
            child->mode = -1;
            child->mode_data = NULL;
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
        }
        // On a close parenthesis, we go back to the parent node and set
        // that the next node added will be on the right
        else if (tree_encoding[i] == ')'){
            active = active->parent;
            next_is_right = true;
        }
    }
}

const char* discover_network(){
    // Send discovery command
    Serial.println("1");

    // Wait to see if acknowledgement arrives
    char *resp = readSerial(DISCOVERY_HANDSHAKE_TIMEOUT);
    if(resp == NULL){
        Log::println("Network discovery acknowledgement timeout; no panels connected");
        free(resp);
        return "()";
    }
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

    parse_tree(tree);
    return tree;
}

const char* request_panel_state(char* directions){
    // Construct status command & send it
    char *cmd = (char*)malloc(strlen(directions) + 2);
    sprintf(cmd, "2%s", directions);
    Log::print("Sending command: "); Log::println(cmd);
    Serial.println(cmd);

    // Wait for response
    char *resp = readSerial(DISCOVERY_RESPONSE_TIMEOUT);
    if(resp == NULL || strcmp(resp, ERR_PANEL_STATE_RESPONSE) == 0){
        free(resp);
        return ERR_PANEL_STATE_RESPONSE;
    }
    return resp;
}
