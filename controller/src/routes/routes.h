#ifndef ROUTES_H
#define ROUTES_H

#include <stdlib.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

#include "../utils/utils.h"
#include "../utils/constants.h"


/**
 * Recursively discovers the topology of the panel network
 * --------------------------------------------------------------
 * Sends a 'discover' command to the first panel, which, if connected,
 * replies immediately with "Acknowledged!". That panel then sends the same
 * discover command to both of its neighbors, and waits to see if they
 * acknowledge, etc..
 * When this recursion reaches a leaf (neither neighbor acknowledges), the leaf replies
 * that it has no neighbors ("(XX)"). Its parent then combines the replies of both its children
 * and sends it back up to its parent ("((XX)X)" if the node has a leaf to the left and nothing to the right), etc..
 * This continues back up the tree until the first panel hands the completed string back to the controller
 */
void discover_network();

void get_panel_state();

void set_panel_mode();

/**
 * Sets a panel to a given solid color
 * --------------------------------------------------------------
 */
void set_panel_color();

void set_panel_customgradient();

#endif
