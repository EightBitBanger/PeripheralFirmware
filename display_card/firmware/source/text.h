// Text glyph rendering functions
//

// Cursor & counters
uint8_t cursor_line = 0;
uint8_t cursor_position = 0;
uint16_t cursorCounter = 0;
uint8_t  cursorState = 0;

// Text
static inline void setPosition(uint8_t position);
static inline void setLine(uint8_t line);
static inline void clearScreen(void);
static inline void shiftFrameUp(void);




static inline void setPosition(uint8_t position) {
    PORTA = 0b01000000 + position;
    PORTD = 0b00011001;
    PORTD = 0b00011000;
    _delay_us(10);
    PORTD = 0b00000001;
    return;
}

static inline void setLine(uint8_t line) {
    PORTA = 0b10111000 + line;
    PORTD = 0b00011001;
    PORTD = 0b00011000;
    _delay_us(10);
    PORTD = 0b00000001;
    return;
}

void clearScreen(void) {
    
    for (uint8_t l=0; l < 8; l++) {
        setLine(l);
        setPosition(0);
        
        for (uint8_t a=0; a < 64; a++) {
            PORTA = 0x00;
            PORTD = 0b00011101;
            PORTD = 0b00011100;
            _delay_us(10);
            PORTD = 0b00000101;
        }
    }
    
    setLine(0);
    setPosition(0);
    return;
}

static inline void shiftFrameUp(void) {
    // Shift up the VRAM buffer
    memmove(frameBuffer.back, frameBuffer.back + 128, 128 * 7);
    // Clear last line
    memset(frameBuffer.back + (7 * 128), ' ', 128);
    return;
}

