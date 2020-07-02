#ifndef TREE_H
#define TREE_H


typedef struct {
    Node* parent;
    Node* left;
    Node* right;
    char* directions;
} Node;

/**
 * Algorithm to parse the network structure from the given string
 */
Node* parse_tree(char* tree){
    int input_length = strlen(tree);

    Node* root = (Node*)malloc(sizeof(Node));
    root->directions = "";
    root->parent = NULL;
    root->left = NULL;
    root->right = NULL;

    Node* active = root;
    boolean next_is_right = false;
    int active_directions_length = 1;
    for(int i = 1; i < input_length-1; i++){
        // On an open parenthesis, we add a new node to the tree
        if(tree[i] == '('){
            Node *child = (Node*)malloc(sizeof(Node));
            child->parent = active;
            child->directions = (char*)malloc(active_directions_length+1);
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
        else if (tree[i] == ')'){
            active = active->parent;
            next_is_right = true;
        }
    }

    return root;
}

#endif