#include <string.h>
#include <stdlib.h>

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "utils.h"
#include "constants.h"


const char* WIFI_SSID = "";
const char* WIFI_PASS = "";

const boolean DEBUG = true;
SoftwareSerial debugSerial(6,7);
struct {
    void print(const char* msg)         { if(DEBUG) debugSerial.print(msg);     }
    void println(void)                  { if(DEBUG) debugSerial.println();      }
    void println(const char* msg)       { if(DEBUG) debugSerial.println(msg);   }
    void println(const Printable& x)    { if(DEBUG) debugSerial.print(x);       }
} Log;

ESP8266WebServer server(80);
MDNSResponder* mdns;

void setup() {
    Serial.begin(115200);
    while(!Serial){;}
    Log.println("Starting up");

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.config(IPAddress(192,168,0,231), IPAddress(192,168,0,1), IPAddress(255,255,255,0));

    Log.print("Connecting");
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Log.print(".");
    }
    Log.println();

    Log.print("Connected, IP address: ");
    Log.println(WiFi.localIP());

    mdns = new MDNSResponder();
    if(!mdns->begin("lightpanels", WiFi.localIP())){
        Log.println("ERROR");
    }

    server.onNotFound([](){ server.send(404); });
    server.on("/", [](){ server.send(200); });
    server.on("/network", [](){
        char* tree = discover_network();
        send_response(200, tree);
        free(tree);
    });
    server.on("/panels/color", HTTP_POST, [](){
        StaticJsonDocument<500> data;
        DeserializationError err = deserializeJson(data, server.arg("plain"));
        
        if(err){
            Log.println("ERROR DESERIALIZING");
            Log.println(err.c_str());
        }

        char cmd[22];
        sprintf(cmd, "set_color %s %s %s",
            data["r"].as<String>().c_str(),
            data["g"].as<String>().c_str(),
            data["b"].as<String>().c_str());
        send_command(data["directions"].as<String>().c_str(), cmd);
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

void send_command(const char* directions, char* cmd){
    // If we have directions, we need to append the forward command with them
    if(strcmp(directions, "") != 0){
        char* fwdCmd = (char*)malloc(strlen(cmd) + strlen(directions) + 4);
        sprintf(fwdCmd, "fwd %s %s", directions, cmd);
        Serial.println(fwdCmd);
        free(fwdCmd);
    }
    // Else this is intended for the first panel, so no need for forwarding
    else Serial.println(cmd);
}

char* discover_network_DEBUG(){
    const char* input = "(((XX)X)(X((XX)X)))";
    char* tree = (char*)malloc(strlen(input));
    strcpy(tree, input);
    return tree;
}

char* discover_network(){
    // Send discovery command
    Serial.println("discover");

    // Read characters until we get a full message and check
    // if it's the acknowledgement
    char *resp = readSerial(DISCOVERY_HANDSHAKE_TIMEOUT);
    if(strcmp(resp, "Acknowledged!") != 0){
        free(resp);
        return NULL;
    }
    free(resp);

    // After discovery acknowledgement, wait for response with full network tree
    return readSerial(DISCOVERY_RESPONSE_TIMEOUT);
}
