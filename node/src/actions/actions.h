/**
 * This file contains the implementations of the actions the panel can take
 * upon receiving different commands, as defined in the main file
 * 
 * Commands:
 *  - `fwd`: Forwards another command to a panel further down the tree
 *  - `discover`: Maps the tree structure of all panels to the left and right, returning it to the caller
 *  - `get_version`: Returns the version of the software
 *  - `set_color`: Sets all the LEDs to the given RGB value
 */
#ifndef ACTIONS_H
#define ACTIONS_H

#include <string.h>
#include <inttypes.h>

#include <SoftwareSerial.h>

#include "../leds/leds.h"
#include "../utils/constants.h"


/**
 * This command is used to send other commands to specific nodes in the tree by
 * forwarding them along a path through the connecting nodes
 * Command format: `0<directions>|<command>`
 *  0 is the forward command, it's dropped when the command is passed into this function,
 *      so the `data` string starts with the first direction
 *  <directions> is a string of L's and R's, dictating which direction to turn
 *      at each node in the tree. When a node receives this command it pops the
 *      first character from the string, and forwards the request in that direction
 *  <command> is the command to be executed by the target node, can be any other valid command
 */
const char* forward_cmd(uint8_t port, char* data);

/**
 * Recursive Network Discovery ---------------------------------------------------------------
 * When a node receives this command it immediately replies "Acknowledged!" to let the caller
 * know they're present, and then sends the same command to its left and right neighbors. When
 * it receives their full responses, it concatenates them, surrounds them with parenthesis, and
 * sends them back to the caller (a neighbor that doesn't reply is replaced with an 'X').
 * 
 * When the master unit sends this command to its first node, it effectively maps the entire network
 * 
 * Example:
 *            O
 *           /             
 *   M*O -- O -- O    ------->      (((XX)(XX))(XX))
 *     |
 *     O
 */
char* discover_network(uint8_t port);

/**
 * Fetch Lighting State ----------------------------------------------------------------------
 * This command allows the controller to fetch the current lighting state of a given panel
 * The controller issues this command one by one to each panel on startup, after network discovery
 * The data is then returned to the app, where the current lighting configuration can be rendered
 *  for the user
 * 
 * Command format: `2<directions>`
 *  <directions> is in the same format as for the forwarding command
 */
void fetch_state_action(uint8_t port, char* data);

/**
 * Sets all of the LEDs to the given color
 * Command format: `2#<hex value>` OR `2<rgb value>
 *  'hex value' is the desired hexadecimal color code (e.x. 'FFFFFF')
 *  'rgb value' is the desired rgb color value padded to make each number 3 digits (e.x. '255000120')
 */
void set_solid_color_action(char* data);

/**
 * Sets the LED strip to the given predefined pattern by index
 * Command format: `3<pattern>`
 *  Where pattern is a number corresponding to a predefined pattern
 */
void set_mode_action(char* data);

/**
 * Sets the ms delay used in updating the LED pattern
 * Command format: `4<speed>`
 *  Where speed is the number of ms to delay LED pattern updates
 */
void set_refresh_rate_action(char* data);

/**
 * Changes the stored custom gradient pattern
 * Command format: `5<length><r><g><b><transition>[<r><g><b><transition>][<r><g><b><transition>]...`
 *  length is the number of steps in the gradient (i.e. how many sets of <r><g><b><transition> will follow)
 *  r, g, and b are hex color codes, and transition is the number of ms (four digits,
 *      zero padded if necessary) between this rgb step and the next
 */
void set_gradient_action(char* data);

#endif
