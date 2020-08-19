#ifndef PROJ_UTILS_H
#define PROJ_UTILS_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "constants.h"


// Logging utilities
class Log {
    static SoftwareSerial *debugSerial;
    public:
        static void begin()                        { if(DEBUG) debugSerial->begin(9600);    }
        static void print(int msg)                 { if(DEBUG) debugSerial->print(msg);     }
        static void print(const char* msg)         { if(DEBUG) debugSerial->print(msg);     }
        static void println(void)                  { if(DEBUG) debugSerial->println();      }
        static void println(int msg)               { if(DEBUG) debugSerial->println(msg);   }
        static void println(const char* msg)       { if(DEBUG) debugSerial->println(msg);   }
        static void println(const Printable& x)    { if(DEBUG) debugSerial->println(x);     }
};

/**
 * Reads a line from the given serial port, up until either the first "\n"
 * or until the optional timeout has passed
 * `timeout` can be set to -1 to run until first "\n" with no bound
 * Adds a string terminator to the input, since serial monitors usually
 * don't send those
 * Returns a malloc'd char pointer, don't forget to free it when you're done
 */
char* readSerial(int timeout);

#endif
