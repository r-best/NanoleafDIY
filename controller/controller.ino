#include <string.h>
#include <stdlib.h>

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "routes.h"
#include "config.h"
#include "constants.h"


MDNSResponder* mdns;

void setup() {
    // Initialize serial
    Serial.begin(115200);
    while(!Serial){;}
    Log.begin();
    Log.println("Starting up");

    // Connect to WiFi network defined in "config.h"
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.config(IPAddress(192,168,0,231), IPAddress(192,168,0,1), IPAddress(255,255,255,0));

    Log.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Log.print(".");
    }
    Log.println();

    Log.print("Connected, IP address: ");
    Log.println(WiFi.localIP());

    // Start MDNS, even though it's pointless because Android won't recognize it :(
    mdns = new MDNSResponder();
    if(!mdns->begin("lightpanels", WiFi.localIP()))
        Log.println("Error starting MDNS");

    // Establish server routes
    server.onNotFound([](){ server.send(404); });
    server.on("/", [](){
        Log.println("Incoming request: Health Check");
        server.send(200);
    });
    server.on("/network", discover_network);
    server.on("/panels/color", HTTP_POST, [](){
        Log.println("Incoming request: Set Panel Color");
        StaticJsonDocument<500> data;
        DeserializationError err = deserializeJson(data, server.arg("plain"));
        
        if(err){
            Log.println("ERROR DESERIALIZING");
            Log.println(err.c_str());
        }

        char cmd[22];
        sprintf(cmd, "2%s%s%s",
            data["r"].as<String>().c_str(),
            data["g"].as<String>().c_str(),
            data["b"].as<String>().c_str());
        send_command(data["directions"].as<String>().c_str(), cmd);
        server.send(200);
    });
    server.on("/panels/customgradient", HTTP_POST, [](){
        Log.println("Incoming request: Set Custom Gradient");
        StaticJsonDocument<500> data;
        DeserializationError err = deserializeJson(data, server.arg("plain"));
        
        if(err){
            Log.println("ERROR DESERIALIZING");
            Log.println(err.c_str());
        }

        int length = strtol(data["length"].as<String>().c_str(), NULL, 10);

        char *cmd = (char*)malloc(length*10+3);
        sprintf(cmd, "5%d", length);
        for(int i = 0; i < length; i++){
            sprintf(cmd+2+(i*10), "%s%s%s%04d",
                data["steps"][i]["r"].as<String>().c_str(),
                data["steps"][i]["g"].as<String>().c_str(),
                data["steps"][i]["b"].as<String>().c_str(),
                data["steps"][i]["t"].as<int>());
        }
        cmd[5+(length*10)] = '\0';
        send_command(data["directions"].as<String>().c_str(), cmd);
        server.send(200);
    });

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
 * Helper method to send an HTTP response to the client with
 * the given status code and data
 */
void send_response(int code, const char* data){
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
void send_command(const char* directions, char* cmd){
    // If we have directions, we need to use the forward command
    if(strcmp(directions, "") != 0){
        char* fwdCmd = (char*)malloc(strlen(cmd) + strlen(directions) + 4);
        sprintf(fwdCmd, "fwd %s %s", directions, cmd);
        Log.print("Sending command: ");Log.println(fwdCmd);
        Serial.println(fwdCmd);
        free(fwdCmd);
    }
    // Else this is intended for the first panel, so no need for forwarding
    else{
        Log.print("Sending command: ");Log.println(cmd);
        Serial.println(cmd);
    }
}
