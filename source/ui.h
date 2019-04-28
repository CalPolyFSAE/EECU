#include "uart.h"

typedef struct uidata_t {
    uint8_t faults[8];
    uint8_t redunRelays : 2;
    uint8_t precharge : 1;
    uint8_t discharge : 1;
    uint8_t airplus : 1;
    uint8_t airminus : 1;
} uidata_t;

const uint8_t mclear[] = "\e[2J";

void uiinit();
void uiupdate();
