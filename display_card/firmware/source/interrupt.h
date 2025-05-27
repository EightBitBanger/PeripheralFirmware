#ifndef _INT_HANDLER__
#define _INT_HANDLER__

// Card select interrupt handler
#define _CONTROL_IN__     PINB
#define _SIGNAL_READ__    1
#define _SIGNAL_WRITE__   0

#define _ADDRESS_IN__     PINC
#define _BUS_DIR__        PORTB
#define _BUS_OUT__        PORTA
#define _BUS_IN__         PINA
#define _BUS_DDR__        DDRA

union Resigner {
    uint8_t unsign;
    int8_t sign;
};

static inline void shiftFrameUp(void);

static uint8_t mutex = 0;
static uint8_t command_issued = 0;

ISR (INT2_vect) {
	
	if (command_issued != 0) 
        return;
    
	if (mutex == 1) 
        return;
    mutex = 1;
    
    // READ request
	if ((_CONTROL_IN__ & (1 << _SIGNAL_READ__)) != (1 << _SIGNAL_READ__)) {
		
		_BUS_DIR__ = 0b00001000; // Output on data bus
		_BUS_DDR__ = 0xff;
		
		if (_ADDRESS_IN__ <= 0x10) {
            _BUS_OUT__ = deviceROM[_ADDRESS_IN__];
		} else {
            
            // Access registry state
            _BUS_OUT__ = frameBuffer.registery[_ADDRESS_IN__ - 0x10];
            
        }
        
        // Wait until deselected
		while ((_CONTROL_IN__ & (1 << _SIGNAL_READ__)) != (1 << _SIGNAL_READ__)) __asm__("nop");
		
		_BUS_DIR__ = 0b00011000; // Disable the bus
		
		mutex = 0;
		return;
	}
	
	// WRITE request
	if ((_CONTROL_IN__ & (1 << _SIGNAL_WRITE__)) != (1 << _SIGNAL_WRITE__)) {
		
		_BUS_DIR__ = 0b00000000; // Input from the bus
		_BUS_DDR__ = 0x00;
		
		// Write the byte to device RAM
		if (_ADDRESS_IN__ >= 0x10) {
            
            // Normal character placement
            
            switch (frameBuffer.registery[7]) {
            default:
                
            case 0: // VRAM
                uint32_t address_offset = frameBuffer.registery[2] + (frameBuffer.registery[1] * 128);
                frameBuffer.back[_ADDRESS_IN__ + address_offset - 0x10] = _BUS_IN__;
                break;
            // Character ram is now part of program memory
            //case 1: { // CHAR_RAM
            //    uint32_t address_offset = frameBuffer.registery[2] * 6;
            //    char_rom[_ADDRESS_IN__ + address_offset - 0x10] = _BUS_IN__;
            //    break;
            //    }
                
            case 2: { // SPRITE_SHEET
                uint32_t address_offset = frameBuffer.registery[2];
                sprite_sheet[ ((_ADDRESS_IN__ - 0x10) * 8) + address_offset ] = _BUS_IN__;
                break;
                }
                
            case 3: { // VERTEX_BUFFER
                uint32_t address_offset = frameBuffer.registery[2];
                union Resigner signswap;
                signswap.unsign = _BUS_IN__;
                vertex_buffer[ (_ADDRESS_IN__ - 0x10) + address_offset ] = signswap.sign;
                break;
                }
            case 4: { // UNIFORM_VARIABLES
                uint32_t address_offset = frameBuffer.registery[2];
                frameBuffer.uniforms.bytes[ (_ADDRESS_IN__ - 0x10) + address_offset ] = _BUS_IN__;
                break;
                }
            }
            
		} else {
            
            frameBuffer.registery[_ADDRESS_IN__] = _BUS_IN__;
            
            if (frameBuffer.registery[5] != 0) 
                command_issued = 1;
            if (frameBuffer.registery[9] != 0) 
                command_issued = 1;
            if (frameBuffer.registery[0x0E] != 0) 
                command_issued = 1;
            if (frameBuffer.registery[0x0F] != 0) 
                command_issued = 1;
            
        }
		
		_BUS_DDR__ = 0xff;
		
		_BUS_DIR__ = 0b00011000; // Disable the bus
		
		mutex = 0;
		return;
	}
	
	mutex = 0;
	return;
}

#endif
