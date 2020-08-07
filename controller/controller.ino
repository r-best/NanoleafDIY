#include <stdlib.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "config.h"
#include "src/utils/utils.h"
#include "src/network/tree.h"
#include "src/routes/routes.h"
#include "src/utils/constants.h"


MDNSResponder* mdns;
ESP8266WebServer server(80);

void setup() {
    // Initialize serial
    Serial.begin(115200);
    while(!Serial){;}
    Log::begin();
    Log::println("Starting up");

    // Connect to WiFi network defined in "config.h"
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.config(IPAddress(192,168,0,231), IPAddress(192,168,0,1), IPAddress(255,255,255,0));

    Log::print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Log::print(".");
    }
    Log::println();

    Log::print("Connected, IP address: ");
    Log::println(WiFi.localIP());

    // Start MDNS, even though it's pointless because Android won't recognize it :(
    mdns = new MDNSResponder();
    if(!mdns->begin("lightpanels", WiFi.localIP()))
        Log::println("Error starting MDNS");

    // Establish server routes
    server.onNotFound([](){ server.send(404); });
    server.on("/", HTTP_GET, [](){
        Log::println("Incoming request: Health Check");
        server.send(200);
    });
    server.on("/network",               HTTP_GET,   get_network_configuration);
    server.on("/network/refresh",       HTTP_GET,   refresh_network_configuration);
    server.on("/panels/state",          HTTP_POST,  get_panel_state);
    server.on("/panels/mode",           HTTP_POST,  set_panel_mode);
    server.on("/panels/color",          HTTP_POST,  set_panel_color);
    server.on("/panels/customgradient", HTTP_POST,  set_panel_customgradient);

    // Obtain urrent panel configuration & light settings
    network_startup();

    server.begin();
    mdns->addService("http", "tcp", 80);
}

/**
 * Main control loop; 
 */
void loop() {
    mdns->update();
    server.handleClient();
}

/**
 * Helper method called on startup to collect information on the connected panels
 * First calls network discovery to learn tree structure of panel network, then
 * requests the current light settings from each panel it discovers, storing all
 * the information to be reported to the app later
 */
void network_startup(){
    Log::println("Beginning startup network discovery");
    while(true){
        Log::println("Looking for panels...");
        const char* tree = discover_network();
        if(strcmp(tree, ERR_NON_ACKNOWLEDGEMENT) == 0){
            Log::print(ERR_NON_ACKNOWLEDGEMENT);Log::print(": ");Log::println(tree);
        }
        else if(strcmp(tree, ERR_READ_TIMEOUT) == 0)
            Log::println(ERR_READ_TIMEOUT);
        else if(strcmp(tree, "()") == 0)
            Log::println("No panels connected");
        else
            break;
        delay(3000);
    }

    // Iterate through the tree, requesting each panel's current lighting state along the way and storing it
    Log::println("Discovered panel network, requesting lighting state");
    bool right = false;
    Node *current = root;
    while(true){
        if(current == root && current->mode > -1) break; // If we've come back up to the root after processing it, we've traversed entire tree

        if(current->mode == -1){ // Prevent processing same node twice
            const char *resp = request_panel_state(current->directions);
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
            Log::print("Panel ");Log::print(current->directions);Log::print(" - Mode ");Log::println(current->mode);
        }

        if      (current->left   != NULL && current->left->mode  == -1) current = current->left;
        else if (current->right  != NULL && current->right->mode == -1) current = current->right;
        else if (current->parent != NULL)                               current = current->parent;
    }
    Log::println("Lighting state obtained, startup complete");
}
