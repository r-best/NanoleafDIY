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
        sprintf(fwdCmd, "0%s|%s", directions, cmd);
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

    // Find correct Node in cached linked list network representation & update its stored settings
    Node *panel = fetch_panel(data["directions"].as<String>().c_str());
    if(panel == NULL){
        send_response(400, "Panel does not exist");
        return;
    }

    // Return the Node's cached config data
    char *buffer = (char*)malloc(strlen(panel->palette) + 7);
    sprintf(buffer, "%d%03d%d%d%s", panel->mode, panel->brightness, panel->randomize, panel->synchronize, panel->palette);
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

    const char* directions = data["directions"].as<String>().c_str();
    uint8_t mode = strtol(data["mode"].as<String>().c_str(), NULL, 10);

    // Send command to panel to switch mode
    char *cmd = (char*)malloc(3); // Malloc not necessarily needed here, but was getting a weird memory issue with a normal char[3], it overlapped with directions somehow
    sprintf(cmd, "4%d", mode);
    send_command(directions, cmd);
    free(cmd);

    // Find correct Node in stored linked list network representation & update its stored settings
    Node *panel = fetch_panel(directions);
    if(panel == NULL){
        send_response(400, "Panel does not exist");
        return;
    }
    panel->mode = mode;

    send_response(200, "");
}

void set_panel_brightness(){
    Log::println("Incoming request: Set Panel Brightness");

    StaticJsonDocument<1000> data;
    if(_parse_input(&data)){
        send_response(500, ERR_PARSE_REQ_BODY);
        return;
    }

    const char* directions = data["directions"].as<String>().c_str();

    char cmd[3];
    sprintf(cmd, "5%s", data["brightness"].as<String>().c_str());
    send_command(directions, cmd);

    send_response(200, "");
}

void set_panel_palette(){
    Log::println("Incoming request: Set Custom Gradient");

    StaticJsonDocument<1000> data;
    if(_parse_input(&data)){
        send_response(500, ERR_PARSE_REQ_BODY);
        return;
    }

    // Find correct Node in stored linked list network representation & update its stored settings
    Node *panel = fetch_panel(data["directions"].as<String>().c_str());
    if(panel == NULL){
        send_response(400, "Panel does not exist");
        return;
    }

    panel->randomize = data["randomize"].as<bool>();
    panel->synchronize = data["synchronize"].as<bool>();
    panel->length = strtol(data["length"].as<String>().c_str(), NULL, 10);
    panel->palette = (char*)realloc(panel->palette, panel->length*10+1);
    for(int i = 0; i < panel->length; i++){
        sprintf(panel->palette+(i*10), "%02s%02s%02s%04d",
            data["steps"][i]["r"].as<String>().c_str(),
            data["steps"][i]["g"].as<String>().c_str(),
            data["steps"][i]["b"].as<String>().c_str(),
            data["steps"][i]["t"].as<int>()
        );
    }
    panel->palette[panel->length*10] = '\0';

    char *cmd = (char*)malloc(panel->length*10+5);
    sprintf(cmd, "6%d%d%d%s", panel->randomize, panel->synchronize, panel->length, panel->palette);
    send_command(data["directions"].as<String>().c_str(), cmd);
    send_response(200, "");
    free(cmd);
}
