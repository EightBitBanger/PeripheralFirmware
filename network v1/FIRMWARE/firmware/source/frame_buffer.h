/*
	UART frame data buffer

ID	    Bit Rate	UBRR (dec)	    UBRR (hex)		Actual  Bit Rate	   Error %

0	    900			1388			0x056C			899.93					  0%
1	    1200		1041			0x0411			1199.62					  0%
2	    2400		520				0x0208			2399.23					  0%
3	    4800		259				0x0103			4807.69					0.2%
4	    9600		129				0x0081			9615.38					0.2%
5	    28.8K		 42				0x002A			29.07 K					0.9%
6	    33.6K		 36				0x0024			33.784 K				0.5%
7	    56.0K	     21	            0x0015	        56.818 K	            1.5%
8       76.8K	     15	            0x000F	        78.125 K	            1.7%
9      125.0K	      8	            0x0008	        138.889 K	           -0.8%

*/

struct FrameBuffer {
	
	// ROM
	volatile uint8_t deviceROM[0x0a] = {0x14,'n','e','t','w','o','r','k',' ', ' '};
	
	volatile uint8_t transmit_flag;              // 0x0a  Request to send the TX frame
	volatile uint8_t receive_flag;               // 0x0b  Indication an incoming RX frame is awaiting processing
	
	volatile uint8_t frame_TX;                   // 0x0c  Transmitter frame buffer
	
	volatile uint8_t UART_init_flag;             // 0x0d  Baud rate initiation flag
	volatile uint8_t UART_baud_rate;             // 0x0e  Baud rate selection register
	
	volatile uint8_t PADDING;                    // 0x0f  
	
	// Frame buffer
	volatile uint8_t frame_RX[200];              // 0x10  Receiver frame buffer array
	
	FrameBuffer() {
		
		transmit_flag             = 0x00;
		receive_flag              = 0x00;
		
	}
	
	// Transmit the TX packet
	uint8_t UART_send(void) {
		
		// Check if a frame is currently being sent
		if (!(UCSR0A & (1<<UDRE0))) return 0;
		
		cli();
		
		// Send the frame
		UDR0 = frame_TX;
		
		sei();
		
		return 1;
	}
	
	// Check if data was received
	uint8_t UART_receive(void) {
		
		// Check if a frame is currently being received
		if (!(UCSR0A & (1<<RXC0))) return 0;
		
		cli();
		
		// Receive the frame
		frame_RX[receive_flag] = UDR0;
		receive_flag++; // Increment the frame stack counter
		
		sei();
		
		return 1;
	}
	
	void UART_flush(void) {
		
		uint8_t dummy = 0x00;
		while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
		
	}
	
} frameBuffer;


