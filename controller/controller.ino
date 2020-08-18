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

    // Obtain current panel configuration & light settings
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

        // If there was an error, log it and continue the loop
        if     (strcmp(tree, ERR_NON_ACKNOWLEDGEMENT) == 0){    Log::print(ERR_NON_ACKNOWLEDGEMENT);Log::print(": ");Log::println(tree); }
        else if(strcmp(tree, ERR_READ_TIMEOUT) == 0)            Log::println(ERR_READ_TIMEOUT);
        else if(strcmp(tree, "") == 0)                          Log::println(ERR_NO_PANELS_CONNECTED);
        else if(strcmp(tree, ERR_TREE_PARSE) == 0)              Log::println(ERR_TREE_PARSE);
        else
            break;
        delay(3000);
    }
    Log::println("Panel network mapping complete, starting webserver");
}
