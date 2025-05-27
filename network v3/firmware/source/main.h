#include <avr/io.h>

#include <avr/interrupt.h>
#include <util/delay.h>

// Data bus
#define _BUS_DIR__         DDRA
#define _BUS_OUT__         PORTA
#define _BUS_IN__          PINA

// Address bus
#define _ADDRESS_DIR__     DDRC
#define _ADDRESS_OUT__     PORTC
#define _ADDRESS_IN__      PINC

// Control bus
#define _CONTROL_DIR__     DDRD
#define _CONTROL_OUT__     PORTD
#define _CONTROL_IN__      PIND

#define _SIGNAL_LED_TX__   0x04
#define _SIGNAL_LED_RX__   0x05
#define _SIGNAL_WRITE__    0x06
#define _SIGNAL_READ__     0x07

