#include <SoftwareSerial.h>

char* VERSION = "0.1";

// Common data structure that allows me to treat
// Serial and SoftwareSerial the same way
typedef struct {
    Stream *serialport;
    void (*begin)(unsigned long);
    boolean (*listen)();
    int (*available)();
    int (*read)();
    int (*print)(char*);
} Port;

// Establish ports (first is Serial, other two
// are SoftwareSerials on pins 2/3 and 4/5)
extern void (Stream::*begin)();
extern void (SoftwareSerial::*listen)();
extern void (Stream::*available)();
extern void (Stream::*read)();
extern void (Stream::*print)();
SoftwareSerial port2(2, 3), port3(4, 5);
Port PORTS[3] = {
    {
        .begin =        (void (*)(unsigned long))   (Serial.*begin),
        .listen =       [](){ return true; },       // No equivalent, just return true
        .available =    (int (*)())                 (Serial.*available),
        .read =         (int (*)())                 (Serial.*read),
        .print =      (int (*)(char*))            (Serial.*print),
    },
    {
        .begin =        (void (*)(unsigned long))   (port2.*begin),
        .listen =       (boolean (*)())             (port2.*listen),
        .available =    (int (*)())                 (port2.*available),
        .read =         (int (*)())                 (port2.*read),
        .print =      (int (*)(char*))            (port2.*print),
    },
    {
        .begin =        (void (*)(unsigned long))   (port3.*begin),
        .listen =       (boolean (*)())             (port3.*listen),
        .available =    (int (*)())                 (port3.*available),
        .read =         (int (*)())                 (port3.*read),
        .print =      (int (*)(char*))            (port3.*print),
    }
};



// Number of ms the network discovery algorithm will
// wait for a neighbor to acknowledge the request
int DISCOVERY_HANDSHAKE_TIMEOUT = 1000;
int DISCOVERY_RESPONSE_TIMEOUT = 1000;

// Response message constants
char* SUCCESS = "Success";
char* ERR_INVALID_ARGS = "Command arguments were invalid";
