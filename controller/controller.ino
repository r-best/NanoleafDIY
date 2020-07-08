#include <string.h>
#include <stdlib.h>

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "tree.h"
#include "utils.h"
#include "constants.h"


const char* WIFI_SSID = "";
const char* WIFI_PASS = "";

ESP8266WebServer server(80);
MDNSResponder* mdns;

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

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.config(IPAddress(192,168,0,231), IPAddress(192,168,0,1), IPAddress(255,255,255,0));

    // Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        // Serial.print(".");
    }
    // Serial.println();

    // Serial.print("Connected, IP address: ");
    // Serial.println(WiFi.localIP());

    mdns = new MDNSResponder();
    if(!mdns->begin("lightpanels", WiFi.localIP())){
        Serial.println("ERROR");
    }

    server.onNotFound([](){ server.send(404); });
    server.on("/", [](){ server.send(200); });
    server.on("/network", [](){
        discover_network();
        send_response(200, tree_encoding);
    });
    server.on("/panels/color", HTTP_POST, [](){
        StaticJsonDocument<500> data;
        DeserializationError err = deserializeJson(data, server.arg("plain"));
        
        if(err){
            Serial.println("ERROR DESERIALIZING");
            Serial.println(err.c_str());
        }

        if(data["directions"].as<String>().compareTo("") == 0)
            Serial.printf("set_color %s %s %s\n",
                data["r"].as<String>().c_str(),
                data["g"].as<String>().c_str(),
                data["b"].as<String>().c_str()
            );
        else
            Serial.printf("fwd %s set_color %s %s %s\n",
                data["directions"].as<String>().c_str(),
                data["r"].as<String>().c_str(),
                data["g"].as<String>().c_str(),
                data["b"].as<String>().c_str()
            );
        send_response(200, "Success");
    });

    server.begin();
    mdns->addService("http", "tcp", 80);

    // Wait one second to make sure all connected panels come online
    delay(1000);

    // Discover all connected panels
    // discover_network();
}

/**
 * Main control loop; 
 */
void loop() {
    mdns->update();
    server.handleClient();
}

void send_response(int code, char* data){
    char* response = (char*)malloc(strlen(data) + 15);
    strcpy(response, code == 200 ? "{ \"data\": \"" : "{ \"err\": \"");
    strcat(response, data);
    strcat(response, "\" }");
    server.send(code, "application/json", response);
    free(response);
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
        if(strcmp(resp, "Acknowledged!") == 0){
            free(resp);
            break;
        }
        free(resp);
    }

    // After discovery acknowledgement, wait for response with full network tree
    char *tree = readSerial(DISCOVERY_RESPONSE_TIMEOUT);
    tree_encoding = tree;
    // parse_tree(tree);
}
