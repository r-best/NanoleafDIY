#include <string.h>
#include <stdlib.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "tree.h"
#include "utils.h"
#include "constants.h"


const char* WIFI_SSID = "***REMOVED***";
const char* WIFI_PASS = "***REMOVED***";

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);
    while(!Serial){;}
    Serial.println("Starting up");

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    if(!MDNS.begin("lightpanels")){
        Serial.println("ERROR");
    }

    server.on("/", [](){ server.send(200, "text/plain", "OK"); });
    server.on("/network", [](){
        char* response = build_response(tree_encoding);
        server.send(200, "application/json", response);
        free(response);
    });

    server.begin();
    MDNS.addService("http", "tcp", 80);

    // Wait one second to make sure all connected panels come online
    delay(1000);

    // Discover all connected panels
    discover_network_DEBUG();
}

/**
 * Main control loop; 
 */
void loop() {
    MDNS.update();
    server.handleClient();
}

char* build_response(char* data){
    char* response = (char*)malloc(strlen(data) + 15);
    strcpy(response, "{ \"data\": \"");
    strcat(response, data);
    strcat(response, "\" }");
    return response;
}

void discover_network_DEBUG(){
    const char* input = "(((XX)X)(X((XX)X)))";
    char* tree = (char*)malloc(strlen(input));
    strcpy(tree, input);
    parse_tree(tree);
}

void discover_network(){
    // Send discovery command to connected panel in
    // a loop until it responds "Acknowledged!"
    while(true){
        // Send discovery command
        Serial.println("discover");

        // Read characters until we get a full message and check
        // if it's the acknowledgement
        char *resp = readSerial(-1);
        if(strcmp(resp, "Acknowledged!") != 0){
            free(resp);
            break;
        }
        free(resp);
    }

    // After discovery acknowledgement, wait for response with full network tree
    char *tree = readSerial(DISCOVERY_RESPONSE_TIMEOUT);
    parse_tree(tree);
}
