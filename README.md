# Nanoleaf DIY

A recreation of [Nanoleaf light panels](https://nanoleaf.me/en/consumer-led-lighting/products/smarter-series/nanoleaf-light-panels-smarter-kit/), built for Arduino

![Architecture Diagram](docs/Architecture.png)

## Controller

The controller is built to run on a NodeMcu ESP8266; it connects to one panel (the root) and exposes a REST API that allows users to send commands through it to any panel in the tree

### API Routes

(TBD)

## Node

Each panel is powered by an Arduino Nano; it accepts serial commands from three ports on the sides of the triangle to change the colors and patterns of its LEDs

The serial commands accepted by the panel are always a single-digit command number, followed by that command's arguments, terminated by a line return

### Command List

0. Forwarding command  
    `0<directions>|<target command>`  
    - Used to send any other command to a target panel in the tree using its "directions", i.e. the chain of left and right turns needed to reach it from the controller
    - When any panel receives this command, it pops the first character off the string of directions and sends the command in that direction ('L' or 'R')
    - Example of directions to different panels in an arrangement:
    ![Architecture Diagram](docs/Forwarding_example.png)
    - In this arrangement, to change the color of the bottommost panel to red, the controller would send `0LRLRLLR|5#FF0000` to the root panel
1. Network Discovery  
    `1`
    - Used to recursively discover all panels connected to the tree
    - When a panel receives this command, it sends the same command to its left and right neighbors, concatenates their responses, wraps the whole thing in parentheses, and returns that to its parent
    - If the panel has no neighbors (is a leaf on the tree), it reples `(XX)` to represent that it has nothing to its left or right
    - This results in an encoding of the tree structure that can be deciphered by the controller to figure out the direction string needed to reach each panel
    - For our ongoing example, the encoded structure is as follows: `((((XX)X)((((XX)(((X(XX))(XX))((XX)X)))(XX))))((XX)X))`. (I think, it's hard to do by hand, I'll test it properly when I assemble that many panels)
2. Fetch current state
    `2`
    - This command allows the controller to fetch the current lighting state of a given panel; the panel will reply with its active mode (single digit number) appended with any parameters for that mode
    - e.x. If the panel was in solid color mode and was pure red, the state reply would be `0FF0000` (mode 0 + hex color code for red)
3. Set Mode  
    `3<mode>`
    - Used to change the panel's lighting mode to one of the built-in presets (0 for solid color, 1 for custom gradient, 3 for rainbow, etc.. - see node documentation for full list)
    - For modes that require parameters (e.g. color), the panel saves the last state of each mode in EEPROM so it can pick up where it left off
4. Set Refresh Rate  
    `4<refresh rate>`
    - Sets how often the LEDs are updated; has an impact on the speed of certain patterns, but needs reworking, pattern speed should be its own parameter and refresh rate should be a constant value
5. Set Solid Color  
    `5#<hex value>` OR `5<rgb value>`
    - Used to set parameters for mode 0, solid color
    - Takes in a hex or rgb color value and sets it as the current color for mode 0, and updates the stored value in EERPROM
6. Set Gradient Pattern  
    `6<length><r><g><b><transition>[<r><g><b><transition>][<r><g><b><transition>]...`
    - Used to set parameters for mode 1, custom gradient
    - Takes in a list of gradient steps, consisting of a color and the number of milliseconds it should take to fade to the next color
    - Sets the list as the current settings for mode 1, and updates the stored value in EEPROM
