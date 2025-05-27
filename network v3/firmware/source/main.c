#include "main.h"
#include "network.h"
#include "interrupt.h"


int main(void) {
    
    uint8_t stringROM[] = {0x14,'n','e','t','w','o','r','k',' ', ' '};
    
    for (uint8_t i=0; i < 0x0a; i++) 
        networkBuffer.deviceROM[i] = stringROM[i];
    
    networkBuffer.receive_flag  = 0x00;
    networkBuffer.transmit_flag = 0x00;
	
	// Set bus direction
	_BUS_DIR__     = 0x00;       // IN - Data bus
	_ADDRESS_DIR__ = 0x00;       // IN - Address bus
	_CONTROL_DIR__ = 0b00110000; // IO - Control bus
	
	// Setup hardware interrupt
	EICRA = 0b00100000; // Enable INT2 falling edge
	EIMSK = 0b00000100; // Enable INT2 mask
	
	// Default baud rate
	uint16_t baudrate = 0x0208; // 2400 default baud at 20 Mhz
	
	//
	// Initiate UARTs
	UBRR0H = (unsigned char)(baudrate>>8);
	UBRR0L = (unsigned char)baudrate;
	
	UBRR1H = (unsigned char)(baudrate>>8);
	UBRR1L = (unsigned char)baudrate;
	
	// Enable TX / RX
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UCSR1B = (1<<RXEN1) | (1<<TXEN1);
	
	// Frame structure (8 data bits | 2 stop bits)
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
	
	//
	// LED counters
	uint8_t state_rx    = 1;
	uint8_t state_tx    = 1;
	uint32_t counter_tx = 0;
	uint32_t counter_rx = 0;
	uint32_t counter_max = 1000;
	
	sei(); // Enable interrupts
	
	// Check LEDs
	for (uint8_t i=0; i <= 3; i++) {
		PORTD |= (1 << _SIGNAL_LED_RX__);  _delay_ms(13);
		PORTD &= ~(1 << _SIGNAL_LED_RX__); _delay_ms(13);
		PORTD |= (1 << _SIGNAL_LED_TX__);  _delay_ms(13);
		PORTD &= ~(1 << _SIGNAL_LED_TX__); _delay_ms(13);
	}
	
	while(1) {
		
		//
		// Check receiver
		if (UART_receive() == 1) {
			
			PORTD |= (1 << _SIGNAL_LED_RX__);
			state_rx   = 1;
			counter_rx = 0;
			
		}
		
		//
		// Check transmitter
		if (networkBuffer.transmit_flag != 0x00) {
			
			PORTD |= (1 << _SIGNAL_LED_TX__);
			state_tx   = 1;
			counter_tx = 0;
			
			if (UART_send() != 0) {
				
				networkBuffer.transmit_flag = 0x00;
				
			}
			
		}
		
		// RX state LED
		if (state_rx == 1) {
			counter_rx++;
			if (counter_rx > counter_max) {
                PORTD &= ~(1 << _SIGNAL_LED_RX__);
                state_rx=0;
            }
		}
		
		// TX state LED
		if (state_tx == 1) {
			counter_tx++;
			if (counter_tx > counter_max) {
                PORTD &= ~(1 << _SIGNAL_LED_TX__);
                state_tx=0;
			}
		}
		
		//
		// Check initiate UART baud rate
		if (networkBuffer.UART_init_flag != 0) {
			
			switch (networkBuffer.UART_baud_rate) {
				case 0: {baudrate = 0x056C; counter_max = 0; break;}
				case 1: {baudrate = 0x0411; counter_max = 0; break;}
                default:
				case 2: {baudrate = 0x0208; counter_max = 1000; break;}
				case 3: {baudrate = 0x0103; counter_max = 1000; break;}
				case 4: {baudrate = 0x0081; counter_max = 1000; break;}
				case 5: {baudrate = 0x002A; counter_max = 3000; break;}
				case 6: {baudrate = 0x0024; counter_max = 5000; break;}
				case 7: {baudrate = 0x0015; counter_max = 7000; break;}
				case 8: {baudrate = 0x000f; counter_max = 10000; break;}
				case 9: {baudrate = 0x0008; counter_max = 10000; break;}
			}
			
			UBRR0H = (unsigned char)(baudrate>>8);
			UBRR0L = (unsigned char)baudrate;
			
			UBRR1H = (unsigned char)(baudrate>>8);
			UBRR1L = (unsigned char)baudrate;
			
			networkBuffer.UART_init_flag = 0x00;
		}
		
	}
	
    return 0;
}
