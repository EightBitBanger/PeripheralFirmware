//
// Configuration

#define  _INITIAL_SETUP_TIME__   60 // ms

// Lower IO bus
#define _BUS_LOWER_DIR__  DDRA
#define _BUS_LOWER_OUT__  PORTA
#define _BUS_LOWER_IN__   PINA

// Upper IO bus
#define _BUS_UPPER_DIR__  DDRC
#define _BUS_UPPER_OUT__  PORTC
#define _BUS_UPPER_IN__   PINC

// Control bus
#define _CONTROL_DIR__    DDRB
#define _CONTROL_OUT__    PORTB
#define _CONTROL_IN__     PINB
// Control lines
#define _CONTROL_READ__             0x00 // Read from memory or IO
#define _CONTROL_WRITE__            0x01 // Write to memory or IO
#define _CONTROL_ADDRESS_LATCH__    0x02 // Address latch
#define _CONTROL_7_SEGMENT__        0x03 // 7seg display latch
//#define _CONTROL_   0x04
//#define _CONTROL_   0x05
//#define _CONTROL_   0x06
//#define _CONTROL_   0x07

//
// Peripheral bus
#define _PERIPHERAL_DIR__    DDRD
#define _PERIPHERAL_OUT__    PORTD
#define _PERIPHERAL_IN__     PIND
// Control lines
#define _PERIPHERAL_ENABLE__      0x04
#define _PERIPHERAL_ADDRESS_0__    0x05
#define _PERIPHERAL_ADDRESS_1__    0x06
#define _PERIPHERAL_ADDRESS_2__    0x07

// Operational wait states
#define _MEMORY_READ_WAIT_STATE_1__
#define _MEMORY_READ_WAIT_STATE_2__

//#define _MEMORY_WRITE_WAIT_STATE_1__
//#define _MEMORY_WRITE_WAIT_STATE_2__

//#define _IO_READ_WAIT_STATE_1__  
//#define _IO_READ_WAIT_STATE_2__  
//#define _IO_READ_WAIT_STATE_3__  
//#define _IO_READ_WAIT_STATE_4__  

//#define _IO_WRITE_WAIT_STATE_1__  
//#define _IO_WRITE_WAIT_STATE_2__  
//#define _IO_WRITE_WAIT_STATE_3__  
//#define _IO_WRITE_WAIT_STATE_4__  

#define  _DISABLE_JTAG__   MCUCR |=(1<<JTD); MCUCR |=(1<<JTD)

#define nop  asm("nop")

#define initiate_board  _CONTROL_DIR__=0xff;_CONTROL_OUT__=0xff; _PERIPHERAL_DIR__=0xff; _PERIPHERAL_OUT__=0xff; _BUS_LOWER_DIR__=0xff; _BUS_LOWER_OUT__=0x00; _BUS_UPPER_DIR__=0xff; _BUS_UPPER_OUT__=0x00; _delay_ms(_INITIAL_SETUP_TIME__); segment_display(0xff);

