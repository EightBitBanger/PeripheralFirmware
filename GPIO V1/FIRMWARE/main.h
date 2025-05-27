//
// PORTA - System data bus
// PORTB - Internal control signals
// PORTC - System address and control input
// PORTD - Internal bus

//#define _CLOCK_RATE_MHZ__  1.8432
//#define _CLOCK_RATE_MHZ__  7.62
//#define _CLOCK_RATE_MHZ__  16.0
#define _CLOCK_RATE_MHZ__  20.0

#define _PERIPHERAL_IDENTITY_BYTE__  0xdb

#define _CONTROL_ADDRESS_0__    0x00
#define _CONTROL_ADDRESS_1__    0x01
#define _CONTROL_ADDRESS_2__    0x02

#define _CONTROL_READ__    0x04
#define _CONTROL_WRITE__   0x05
#define _CONTROL_SELECT__  0x06

// Peripheral PORT A - In\Out
#define _IO_SELECT_A_ENABLE__     0x00
#define _IO_SELECT_A_DIRECTION__  0x01

// Peripheral PORT B - In\Out
#define _IO_SELECT_B_ENABLE__     0x02
#define _IO_SELECT_B_DIRECTION__  0x03

// Peripheral PORT C - Out
#define _LATCH_ENABLE_A__         0x05
#define _LATCH_SELECT_A__         0x04

// Peripheral PORT D - Out
#define _LATCH_ENABLE_B__         0x07
#define _LATCH_SELECT_B__         0x06


#define initiate_board  DDRA=0x00;PORTA=0x00; DDRB=0xff;PORTB=0x00; DDRD=0x00;PORTD=0x00; DDRC=0x00;PORTC=0x00

#define F_CPU  (1000000 * _CLOCK_RATE_MHZ__)

#include <avr/io.h>
#include <util/delay.h>

#define nop  asm("nop")