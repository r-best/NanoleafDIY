#include "routes.h"


extern ESP8266WebServer server;

/** Helper method to parse the body of a POST request, returns nonzero on error */
static int _parse_input(StaticJsonDocument<1000> *data){
    if(DeserializationError err = deserializeJson(*data, server.arg("plain"))){
        Log::println("ERROR DESERIALIZING");
        Log::println(err.c_str());
        return 1;
    }
    return 0;
}

/**
 * Helper method to send an HTTP response to the client with
 * the given status code and data
 */
static void send_response(int code, const char* data){
    char* response = (char*)malloc(strlen(data) + 15);
    strcpy(response, code == 200 ? "{ \"data\": \"" : "{ \"err\": \"");
    strcat(response, data);
    strcat(response, "\" }");
    server.send(code, "application/json", response);
    free(response);
}

/**
 * Sends a command to the desired panel
 * If no directions are present, the command is sent as-is to
 * the first panel
 * Otherwise, the command is prefixed with "fwd" and the given
 * directions, which tells the first panel to forward the command
 * to the next panel in the directions list, which then forwards
 * it to the next panel, etc.. until no directions are left
 */
static void send_command(const char* directions, char* cmd){
    // If we have directions, we need to use the forward command
    if(strcmp(directions, "") != 0){
        char* fwdCmd = (char*)malloc(strlen(cmd) + strlen(directions) + 4);
        sprintf(fwdCmd, "fwd %s %s", directions, cmd);
        Log::print("Sending command: ");Log::println(fwdCmd);
        Serial.println(fwdCmd);
        free(fwdCmd);
    }
    // Else this is intended for the first panel, so no need for forwarding
    else{
        Log::print("Sending command: ");Log::println(cmd);
        Serial.println(cmd);
    }
}

void get_network_configuration(){
    Log::println("Incoming request: Get Network Configuration");
    send_response(200, tree_encoding);
}
// Mock network discovery for when I don't have another panel plugged in
void get_network_configuration_DEBUG(){
    send_response(200, "(((XX)X)(X((XX)X)))");
}

void refresh_network_configuration(){
    Log::println("Incoming request: Refresh Network Configuration");

    const char* tree = discover_network();
    if(strcmp(tree, ERR_NON_ACKNOWLEDGEMENT) == 0){
        Log::print(ERR_NON_ACKNOWLEDGEMENT);Log::print(": ");Log::println(tree);
        send_response(500, ERR_NON_ACKNOWLEDGEMENT);
        return;
    }
    if(strcmp(tree, ERR_READ_TIMEOUT) == 0){
        Log::println(ERR_READ_TIMEOUT);
        send_response(500, "Read timeout in network discovery");
        return;
    }

    send_response(200, tree);
}

void get_panel_state(){
    Log::println("Incoming request: Get Panel State");

    StaticJsonDocument<1000> data;
    if(_parse_input(&data)){
        send_response(500, ERR_PARSE_REQ_BODY);
        return;
    }

    // Find correct Node in stored linked list network representation
    Node *current = root;
    const char *directions = data["directions"].as<String>().c_str();
    for(int i = 0; i < strlen(directions); i++){
        if(directions[i] == 'L')
            current = current->left;
        else if(directions[i] == 'R')
            current = current->right;

        // If we just moved to a panel that doesn't exist, return an error
        if(current == NULL){
            send_response(400, "Panel does not exist");
            return;
        }
    }

    // Return the Node's saved config data
    char* buffer = (char*)malloc(strlen(current->mode_data) + 1);
    sprintf(buffer, "%d%s", current->mode, current->mode_data);
    send_response(200, buffer);
    free(buffer);
}

void set_panel_mode(){
    Log::println("Incoming request: Set Panel Mode");

    StaticJsonDocument<1000> data;
    if(_parse_input(&data)){
        send_response(500, ERR_PARSE_REQ_BODY);
        return;
    }

    char cmd[3];
    sprintf(cmd, "3%s", data["mode"].as<String>().c_str());
    send_command(data["directions"].as<String>().c_str(), cmd);
    server.send(200);
}

void set_panel_color(){
    Log::println("Incoming request: Set Panel Color");

    StaticJsonDocument<1000> data;
    if(_parse_input(&data)){
        send_response(500, ERR_PARSE_REQ_BODY);
        return;
    }

    char cmd[22];
    sprintf(cmd, "5%s%s%s",
        data["r"].as<String>().c_str(),
        data["g"].as<String>().c_str(),
        data["b"].as<String>().c_str());
    send_command(data["directions"].as<String>().c_str(), cmd);
    server.send(200);
}

void set_panel_customgradient(){
    Log::println("Incoming request: Set Custom Gradient");

    StaticJsonDocument<1000> data;
    if(_parse_input(&data)){
        send_response(500, ERR_PARSE_REQ_BODY);
        return;
    }

    int length = strtol(data["length"].as<String>().c_str(), NULL, 10);

    char *cmd = (char*)malloc(length*10+3); // Length of step * number of steps, plus two starting digits and a \0
    sprintf(cmd, "6%d", length);
    for(int i = 0; i < length; i++){
        sprintf(cmd+2+(i*10), "%s%s%s%04d",
            data["steps"][i]["r"].as<String>().c_str(),
            data["steps"][i]["g"].as<String>().c_str(),
            data["steps"][i]["b"].as<String>().c_str(),
            data["steps"][i]["t"].as<int>());
    }
    send_command(data["directions"].as<String>().c_str(), cmd);
    server.send(200);
}
