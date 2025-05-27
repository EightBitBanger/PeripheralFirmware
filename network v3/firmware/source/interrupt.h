
// Card select interrupt handler
ISR (INT2_vect) {
	
	// READ request
	if ((_CONTROL_IN__ & (1 << _SIGNAL_READ__)) != (1 << _SIGNAL_READ__)) {
		
		_BUS_DIR__ = 0xff; // Output on data bus
		
		_BUS_OUT__ = networkBuffer.deviceROM[_ADDRESS_IN__];
		
		while ((_CONTROL_IN__ & (1 << _SIGNAL_READ__)) != (1 << _SIGNAL_READ__)) {
			__asm__("nop");
		}
		
		_BUS_DIR__ = 0x00;
		
		return;
	}
	
	// WRITE request
	if ((_CONTROL_IN__ & (1 << _SIGNAL_WRITE__)) != (1 << _SIGNAL_WRITE__)) {
		
		// Write the byte to device RAM
		if (_ADDRESS_IN__ >= 0x0a) networkBuffer.deviceROM[_ADDRESS_IN__] = _BUS_IN__; // Begin after ROM area
		
		return;
	}
	
	return;
}



