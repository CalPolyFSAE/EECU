#include "ui.h"
#include "gpio.h"

using namespace BSP;

uidata_t uidata;

uint8_t screen[100];

uint8_t userInput;

static void hexbyte(uint8_t x, uint8_t* dest){
    uint8_t msn = (x&0xf0)>>4;
    uint8_t lsn = (x&0xf);
    *dest = msn <= 9 ? '0'+msn : 'A'+(msn%10);
    *(dest+1) = lsn <= 9 ? '0'+lsn : 'A'+(lsn%10);
}

void uirx(uint8_t data){
    uart::UART& uart = BSP::uart::UART::StaticClass();

    if(data >= '0' && data <= '9')
        userInput = data - '0';
    
    /*
    if(data == '1'){
        gpio::GPIO::toggle(gpio::PortE, 7);
    } else if(data == '2'){    
        gpio::GPIO::toggle(gpio::PortA, 6);
    } else if(data == '4'){
        gpio::GPIO::toggle(gpio::PortC, 9);
    } else if(data == '5'){
        gpio::GPIO::toggle(gpio::PortB, 13);
    } else if(data == '6'){
        gpio::GPIO::toggle(gpio::PortB, 0);
    }
    */
}

void uiinit(){
    memset(&uidata, 0, sizeof(uidata_t));
    memcpy(screen+0, "faults: \r\n", 10);
    memcpy(screen+10, "precharge: \r\n", 13);
    uart::config uartc;
    uart::UART::ConstructStatic(&uartc);
    uart::UART& uart = BSP::uart::UART::StaticClass();
    uart::UART::uartconfig uart0c;
    uart0c.callback = uirx;
    uart0c.echo = 0;
    uart.init(0, &uart0c);
}

void uiupdate(){
    uart::UART::StaticClass().write(0, mclear, 4);
    uart::UART::StaticClass().write(0, screen, 23);
}



