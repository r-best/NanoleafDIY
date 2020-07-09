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


typedef struct Node {
    Node* parent;
    Node* left;
    Node* right;
    char* directions;
} Node;

char* tree_encoding = NULL;
Node* root = NULL;

/**
 * Recursive funtion to free up all the malloc'd memory
 * used by the network tree, so the tree can be recalculated
 * Should be called with the root node as the argument
 */
void clear_tree(Node* node){
    free(node->directions);
    if(node->left) clear_tree(node->left);
    if(node->right) clear_tree(node->right);
    free(node);
}

/**
 * Algorithm to parse the network structure from the given string
 */
void parse_tree(char* tree){
    tree_encoding = tree;
    int input_length = strlen(tree_encoding);

    if(root != NULL){
        clear_tree(root);
        free(tree_encoding);
    }

    root = (Node*)malloc(sizeof(Node));
    root->directions = (char*)malloc(1);
    strcpy(root->directions, "");
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

#endif
