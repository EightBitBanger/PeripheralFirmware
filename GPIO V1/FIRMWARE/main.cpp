#include "main.h"

int main(void) {
    
	initiate_board;
	
	// Initiate GPIO gateways
	DDRD = 0xff;
	PORTD = 0x00;
	
	// IO - Port A
	PORTB |= (1 << _IO_SELECT_A_ENABLE__);
	PORTB |= (1 << _IO_SELECT_A_DIRECTION__);
	// IO - Port B
	PORTB |= (1 << _IO_SELECT_B_ENABLE__);
	PORTB |= (1 << _IO_SELECT_B_DIRECTION__);
	// O - Port C
	PORTB &= ~(1 << _LATCH_ENABLE_A__);
	PORTB |= (1 << _LATCH_SELECT_A__);
	PORTB &= ~(1 << _LATCH_SELECT_A__);
	// O - Port D
	PORTB &= ~(1 << _LATCH_ENABLE_B__);
	PORTB |= (1 << _LATCH_SELECT_B__);
	PORTB &= ~(1 << _LATCH_SELECT_B__);
	
	DDRD = 0x00;
	
	while (1) {
		
		//
		// Continue until selected
		if ((PINC & (1 << _CONTROL_SELECT__)) == (1 << _CONTROL_SELECT__)) {continue;}
		
		// Catch the address
		uint16_t address = 0x0000;
		address = PINC & 0b00000011;
		
		
		//
		// READ
		if ((PINC & (1 << _CONTROL_READ__)) != (1 << _CONTROL_READ__)) {
			
			// Device identity byte
			if (address == 0x0000) {
				
				DDRA = 0xff; // Output to motherboard
				
				PORTA = _PERIPHERAL_IDENTITY_BYTE__;
				
				// While read is low
				//while( (PINC & (1 << _CONTROL_READ__)) != (1 << _CONTROL_READ__) ) {nop;}
				
				DDRA = 0x00;
				
				continue;
			}
			
			// Read transceiver A
			if (address == 0x0001) {
				
				DDRD = 0x00; // Input on local bus
				DDRA = 0xff; // Output to motherboard
				
				PORTB &= ~(1 << _IO_SELECT_A_DIRECTION__); // Read in on external port A
				PORTB &= ~(1 << _IO_SELECT_A_ENABLE__);
				
				nop;
				
				// Read the byte
				PORTA = PIND;
				
				// WHILE read is low
				//while( (PINC & (1 << _CONTROL_READ__)) != (1 << _CONTROL_READ__) ) {nop;}
				
				PORTB |= (1 << _IO_SELECT_A_ENABLE__);
				
				DDRA = 0x00;
				
				continue;
			}
			
			// Read transceiver B
			if (address == 0x0002) {
				
				DDRD = 0x00; // Input on local bus
				DDRA = 0xff; // Output to motherboard
				
				PORTB &= ~(1 << _IO_SELECT_B_DIRECTION__); // Read in on external port B
				PORTB &= ~(1 << _IO_SELECT_B_ENABLE__);
				
				nop;
				
				// Read the byte
				PORTA = PIND;
				
				// WHILE read is low
				//while( (PINC & (1 << _CONTROL_READ__)) != (1 << _CONTROL_READ__) ) {nop;}
				
				PORTB |= (1 << _IO_SELECT_B_ENABLE__);
				
				DDRA = 0x00;
				
				continue;
			}
			
			continue;
		}
		
		
		//
		// WRITE
		if ((PINC & (1 << _CONTROL_WRITE__)) != (1 << _CONTROL_WRITE__)) {
			
			// Transceiver A
			if (address == 0x0000) {
				
				DDRD = 0xff; // Output on local bus
				DDRA = 0x00; // Input from motherboard
				
				PORTB |= (1 << _IO_SELECT_A_DIRECTION__); // Write on external port A
				PORTB &= ~(1 << _IO_SELECT_A_ENABLE__);
				
				nop;
				
				// Write the byte
				PORTD = PINA;
				
				// WHILE write is low
				//while( (PINC & (1 << _CONTROL_WRITE__)) != (1 << _CONTROL_WRITE__) ) {nop;}
				
				PORTB |= (1 << _IO_SELECT_A_ENABLE__);
				
				DDRA = 0x00;
				
				continue;
			}
			
			// Transceiver B
			if (address == 0x0001) {
				
				DDRD = 0xff; // Output on local bus
				DDRA = 0x00; // Input from motherboard
				
				PORTB |= (1 << _IO_SELECT_B_DIRECTION__); // Write on external port A
				PORTB &= ~(1 << _IO_SELECT_B_ENABLE__);
				
				nop;
				
				// Write the byte
				PORTD = PINA;
				
				// WHILE write is low
				//while( (PINC & (1 << _CONTROL_WRITE__)) != (1 << _CONTROL_WRITE__) ) {nop;}
				
				PORTB |= (1 << _IO_SELECT_B_ENABLE__);
				
				DDRA = 0x00;
				
				continue;
			}
			
			// Latch A
			if (address == 0x0002) {
				
				DDRA=0x00;
				DDRD=0xff;
				
				// Write the byte
				PORTD = PINA;
				
				// Write the byte to the port
				PORTB |= (1 << _LATCH_SELECT_A__);
				nop;
				PORTB &= ~(1 << _LATCH_SELECT_A__);
				
				// Zero the bus
				DDRD=0x00;
				
				// While write is low
				//while( (PINC & (1 << _CONTROL_WRITE__)) != (1 << _CONTROL_WRITE__) ) {nop;}
				
				continue;
			}
			
			// Latch B
			if (address == 0x0003) {
				
				DDRA=0x00;
				DDRD=0xff;
				
				// Write the byte
				PORTD = PINA;
				
				// Write the byte to the port
				PORTB |= (1 << _LATCH_SELECT_B__);
				nop;
				PORTB &= ~(1 << _LATCH_SELECT_B__);
				
				// Zero the bus
				DDRD=0x00;
				
				// While write is low
				//while( (PINC & (1 << _CONTROL_WRITE__)) != (1 << _CONTROL_WRITE__) ) {nop;}
				
				continue;
			}
			
			continue;
		}
		
		
    }
	
}

