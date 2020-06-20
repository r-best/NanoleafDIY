// #include <sys/time.h>
#include <string.h>

#include "led.c"
#include "constants.h"


char* _get_next_arg(){
    return strtok(NULL, " ");
}

char* get_version(uint8_t port){
    return VERSION;
}

/**
 * Recursive network discovery; the main node
 * will send this request to its neighbor, which
 * will check with both of its neighbors, etc..,
 * until the network is fully mapped
 */
char* discover_network(uint8_t port){
    // Let requesting panel know we heard
    PORTS[port].print("Acknowledged!");

    uint8_t left, right;
    if      (port == 0) { left = 1; right = 2; }
    else if (port == 1) { left = 2; right = 0; }
    else if (port == 2) { left = 0; right = 1; }

    // Ask left port to discover, see if it replies
    boolean left_active = _request_discover(left);
    
    // Ask right port to discover, see if it replies
    boolean right_active = _request_discover(right);

    char* left_tree;
    if(left_active) _receive_discover(left, &left_tree);

    char* right_tree;
    if(right_tree) _receive_discover(right, &right_tree);

    // Construct tree string
    
    // Set color to green
}

/**
 * 
 */
void _receive_discover(uint8_t port, char** tree){
    PORTS[port].listen();
    gettimeofday(&start, NULL);
    do {
        int i = 0;
        while(PORTS[port].available > 0){
            *tree[i] = PORTS[port].read();
            i++;
        }
        gettimeofday(&now, NULL);
    } while(now.tv_usec - start.tv_usec < DISCOVERY_RESPONSE_TIMEOUT*1000);
}

/**
 * 
 */
boolean _request_discover(uint8_t port){
    PORTS[port].listen();
    PORTS[port].print("discover");
    gettimeofday(&start, NULL);
    do {
        char* resp;
        int i = 0;
        while(PORTS[port].available > 0){
            resp[i] = PORTS[port].read();
            i++;
        }
        if(strcmp(resp, "Acknowledged!") == 0)
            return true;
        gettimeofday(&now, NULL);
    } while(now.tv_usec - start.tv_usec < DISCOVERY_HANDSHAKE_TIMEOUT*1000);
    return false;
}

char* set_color(uint8_t port){
    uint8_t colors[3] = {0, 0, 0};
    for(int i = 0; i < 3; i++){
        char *temp = _get_next_arg();
        if(temp == NULL) return ERR_INVALID_ARGS;

        int color = atoi(temp);
        if(color < 0 || color > 255) return ERR_INVALID_ARGS;

        colors[i] = color;
    }
    set_solid(colors);
    return SUCCESS;
}
