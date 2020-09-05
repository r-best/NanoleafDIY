#ifndef ROUTES_H
#define ROUTES_H

#include <stdlib.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

#include "../network/tree.h"
#include "../utils/utils.h"
#include "../utils/constants.h"


void get_network_configuration();

void refresh_network_configuration();

void get_panel_state();

void set_panel_mode();

void set_panel_brightness();

/**
 * Sets a panel to a given solid color
 * --------------------------------------------------------------
 */
void set_panel_color();

void set_panel_customgradient();

void set_panel_blink();

#endif
