//
// Liquid Crystal controller firmware

#define F_CPU  20000000UL
#define nop    asm("nop")

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

#include "frame_buffer.h"




// Card select interrupt handler
ISR (INT2_vect) {
	
	// READ request
	if ((_CONTROL_IN__ & (1 << _SIGNAL_READ__)) != (1 << _SIGNAL_READ__)) {
		
		_BUS_DIR__ = 0xff; // Output on data bus
		
		_BUS_OUT__ = frameBuffer.deviceROM[_ADDRESS_IN__];
		
		while ((_CONTROL_IN__ & (1 << _SIGNAL_READ__)) != (1 << _SIGNAL_READ__)) 
			asm("nop");
		
		_BUS_DIR__ = 0x00;
		
		return;
	}
	
	// WRITE request
	if ((_CONTROL_IN__ & (1 << _SIGNAL_WRITE__)) != (1 << _SIGNAL_WRITE__)) {
		
		// Write the byte to device RAM
		if (_ADDRESS_IN__ >= 0x0a) frameBuffer.deviceROM[_ADDRESS_IN__] = _BUS_IN__; // Begin after ROM area
		
		return;
	}
	
	return;
}



