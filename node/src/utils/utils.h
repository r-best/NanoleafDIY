#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include <Arduino.h>

#include "constants.h"


/**
 * Reads a line from the given serial port, up until either the first "\n"
 * or until the optional timeout has passed
 * `timeout` can be set to -1 to run until first "\n" with no bound
 * Adds a string terminator to the input, since serial monitors usually
 * don't send those
 * Returns a malloc'd char pointer, don't forget to free it when you're done
 */
char* readSerial(uint8_t port, int timeout);

#endif
