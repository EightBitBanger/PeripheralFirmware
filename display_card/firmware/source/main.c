#include "main.h"

#include "bitwise.h"

#include <string.h>
#include <stdlib.h>

struct FrameBuffer frameBuffer;
#include "char_rom.h"
#include "sprite_sheet.h"
#include "vertex_buffer.h"
#include "dither.h"

volatile uint8_t deviceROM[] = {0x10, 'd','i','s','p','l','a','y',' ',' ',' ', 
                       '1','2','8','x','6','4' };

#include "interrupt.h"     // Hardware interrupt control


// Text
static inline void setPosition(uint8_t position);
static inline void setLine(uint8_t line);
static inline void clearScreen(void);
static inline void shiftFrameUp(void);

// 2D
static inline void drawPixel(uint8_t x, uint8_t y);
static inline void clearPixel(uint8_t x, uint8_t y);

static inline void drawLine(int x0, int y0, int x1, int y1);
void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void drawQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void drawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void drawCircle(int xc, int yc, int r);
void drawFilledCircle(int xc, int yc, int r);
void drawRotatedRect(int xc, int yc, int w, int h, float angle);

// 3D
void drawLine3D(int x1, int y1, int x2, int y2, int xx, int yy, float angleX, float angleY, float angleZ);
void drawTriangle3D(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, 
                    float xx, float yy, float zz, float angleX, float angleY, float angleZ, float scale);
void drawShadedTriangle3D(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, 
                          float xx, float yy, float zz, float angleX, float angleY, float angleZ, 
                          float scale, int intensity, const uint8_t* ditherPattern, int patternSize);

// Rendering
void swapBuffers(void);

void renderText(void);
void renderGraphics(void);
void renderSprites(void);
void renderLines(void);
void renderVertexBuffer(void);

void renderDrawGlyph(uint8_t* glyph, uint16_t width, uint16_t char_offset);
void renderDrawGlyphText(uint16_t width, uint16_t char_offset);


#include "2D.h"            // 2D drawing functions
#include "3D.h"            // 3D drawing and rasterization functions
#include "text.h"          // Text glyph functions
#include "depth_buffer.h"  // Block depth buffer


uint8_t init=0;

int main(void) {
    
    // Data bus
    DDRA = 0xff;
    PORTA = 0x00;
    
    // Address bus
    DDRC = 0x00;
    PORTC = 0x00;
    
    // 0 - Write
    // 1 - Read
    // 2 - Interrupt select
    // 3 - Data bus direction
    // 4 - Data bus enable
    
    DDRB  = 0b00011000;
    PORTB = 0b00011000;
    
    // 0 - Display Enable (Active low)
    // 1 - 1=Read/0=write
    // 2 - Register select 1=data 0=instruction
    // 3 - CS2
    // 4 - CS1
    
    DDRD  = 0b00011111;
    PORTD = 0b00000001;
    
    // Allow display some time to initiate
    _delay_ms(100);
    
    // Initiate video ram
    for (uint16_t i=0; i < 1024; i++) 
        frameBuffer.vram[i] = 0x00;
    
    // Clear the display
    clearScreen();
    
    frameBuffer.registery[0x00] = 0x00;
    frameBuffer.registery[0x01] = 0x00;
    frameBuffer.registery[0x02] = 0x00;
    frameBuffer.registery[0x03] = 50;
    frameBuffer.registery[0x04] = '_';
    frameBuffer.registery[0x05] = 0;
    frameBuffer.registery[0x06] = 0;
    frameBuffer.registery[0x07] = 0;
    frameBuffer.registery[0x08] = 4;
    frameBuffer.registery[0x09] = 0;
    frameBuffer.registery[0x0A] = 0;
    frameBuffer.registery[0x0B] = 0;
    frameBuffer.registery[0x0C] = 0;
    frameBuffer.registery[0x0D] = 0;
    frameBuffer.registery[0x0E] = 0;
    frameBuffer.registery[0x0F] = 0;
    
	// Initiate the display
    PORTA = 0b00111111;
    
    PORTD = 0b00011001;
    PORTD = 0b00011000;
    _delay_us(100);
    PORTD = 0b00000001;
    
    // Set hardware interrupt
	EICRA = 0b00100000; // Enable INT2 falling edge
	EIMSK = 0b00000100; // Enable INT2 mask
	
	EnableGlobalInterrupts;
    
	while(1) {
        
        switch (frameBuffer.registery[0]) {
        case 0: // Text mode - 8x6 glyphs from character ROM
            swapBuffers();
            renderText();
            break;
        case 1: // Graphics mode - VRAM as a bitmap
            swapBuffers();
            renderGraphics();
            break;
        case 2: // Sprite mode - 8x8 glyphs from sprite sheet
            swapBuffers();
            renderSprites();
            break;
        }
        
        // Check draw a range in the buffer
        if (frameBuffer.registery[0x0E] != 0) {
            frameBuffer.registery[0x0E] = 0;
            
            switch (frameBuffer.registery[0]) {
                
            case 3: // Line render mode - Vertex buffer
                renderLines();
                break;
            case 4: // Triangle render mode - Vertex buffer
                renderVertexBuffer();
                break;
            }
            command_issued = 0;
        }
        
        // Check to shift frame up
        if (frameBuffer.registery[5] != 0) {
            
            frameBuffer.registery[5] = 0;
            
            shiftFrameUp();
            command_issued = 0;
        }
        
        // Check to clear the frame buffer
        if (frameBuffer.registery[9] != 0) {
            frameBuffer.registery[9] = 0;
            
            memset(frameBuffer.back, 0x00, 1024);          // Clear back frame buffer
            memset(depthBuffer, 255, sizeof(depthBuffer)); // Clear depth buffer
            command_issued = 0;
        }
        
        // Check to swap the frame buffers
        if (frameBuffer.registery[0x0F] != 0) {
            frameBuffer.registery[0x0F] = 0;
            command_issued = 0;
            
            swapBuffers(); // Turn over the buffer chain
            
            if (frameBuffer.registery[0] == 3 || frameBuffer.registery[0] == 4) {
                renderGraphics();
                
                continue;
            }
            
        }
        
        continue;
    }
    
    return 0;
}


void renderText(void) {
    
    for (uint32_t l=0; l < 8; l++) {
        
        setLine(l);
        setPosition(0);
        
        cursor_line = l;
        cursor_position = 0;
        
        for (uint32_t c=0; c <= 21; c++) {
            
            // Fill in blank space
            if (c == 21) {
                
                // Two blank spaces
                renderDrawGlyphText(2, 0);
                
                cursor_position -= 2;
                break;
            }
            
            // Render the text glyph
            
            // Get byte from video memory
            uint8_t glyph = frameBuffer.vram[c + (l * 128)];
            
            // Render the glyph
            renderDrawGlyphText(6, glyph);
            
        }
        
        // Cursor
        
        if (frameBuffer.registery[3] > 0) {
            uint16_t cursorMax = (uint16_t)frameBuffer.registery[3] * 4;
            
            // Blink timer
            cursorCounter++;
            if (cursorCounter > cursorMax) {
                cursorCounter=0;
                
                if (cursorState == 1) {
                    cursorState = 0;
                } else {
                    cursorState = 1;
                }
            }
            
            // Draw cursor
            if (cursorState == 1) {
                setLine( frameBuffer.registery[1] );
                
                cursor_position = (frameBuffer.registery[2] * 6);
                if (cursor_position > 63) {
                    setPosition( (frameBuffer.registery[2] * 6) - 64 );
                } else {
                    setPosition( (frameBuffer.registery[2] * 6) );
                }
                
                if (frameBuffer.registery[2] == 10) {
                    renderDrawGlyphText(4, frameBuffer.registery[4]);
                } else {
                    renderDrawGlyphText(6, frameBuffer.registery[4]);
                }
                
            }
            
        }
        
        continue;
    }
    
    return;
}

void renderGraphics(void) {
    cli();
    for (uint32_t l=0; l < 8; l++) {
        
        setLine(l);
        setPosition(0);
        
        cursor_line = l;
        cursor_position = 0;
        
        // Draw raw video memory as a bitmap
        for (uint32_t c=0; c < 16; c++) 
            renderDrawGlyph(frameBuffer.vram, 8, c + (l * 16));
        
        continue;
    }
    sei();
    return;
}

void renderSprites(void) {
    cli();
    for (uint32_t l=0; l < 8; l++) {
        
        setLine(l);
        setPosition(0);
        
        cursor_line = l;
        cursor_position = 0;
        
        // Render glyphs from the sprite sheet
        for (uint32_t c=0; c < 16; c++) 
            renderDrawGlyph( sprite_sheet, 8, frameBuffer.vram[c + (l * 128)] );
        
        continue;
    }
    
    sei();
    return;
}

void renderLines(void) {
    cli();
    
    // Line renderer
    Line ln;
    
    for (uint8_t i=0; i < frameBuffer.registery[8]; i++) {
        uint8_t index = i * 4;
        
        ln.x.x = vertex_buffer[index];
        ln.x.y = vertex_buffer[index+1];
        ln.y.x = vertex_buffer[index+2];
        ln.y.y = vertex_buffer[index+3];
        
        float angleX = 0.0f;
        float angleY = 0.0f;
        float angleZ = 0.0f;
        
        drawLine3D(ln.x.x, ln.x.y, ln.y.x, ln.y.y, 0, 0, angleX, angleY, angleZ);
        
    }
    
    renderSprites();
    
    sei();
    return;
}

void renderVertexBuffer(void) {
    cli();
    Vertex vert;
    if (frameBuffer.registery[8] < 1) return; // Ensure at least one triangle in the buffer
    
    for (uint8_t i=0; i < frameBuffer.registery[0x0D]; i++) {
        // Vertex buffer stride
        uint8_t index = (i + frameBuffer.registery[0x0C]) * 10;
        
        vert.x.x = vertex_buffer[index];
        vert.x.y = vertex_buffer[index+1];
        vert.x.z = vertex_buffer[index+2];
        
        vert.y.x = vertex_buffer[index+3];
        vert.y.y = vertex_buffer[index+4];
        vert.y.z = vertex_buffer[index+5];
        
        vert.z.x = vertex_buffer[index+6];
        vert.z.y = vertex_buffer[index+7];
        vert.z.z = vertex_buffer[index+8];
        uint8_t dither_effect = vertex_buffer[index+9];
        
        // Select dithering pattern based on the dither effect
        uint8_t* ditherPattern = (uint8_t*)DITHER_PATTERN_HIGH;
        int intensity = 100;
        
        switch (dither_effect) {
            case 0: ditherPattern = (uint8_t*)DITHER_PATTERN_HORZ_LINES; break;
            case 1: ditherPattern = (uint8_t*)DITHER_PATTERN_VERT_LINES; break;
            case 2: ditherPattern = (uint8_t*)DITHER_PATTERN_LOW; intensity = 90; break;
            case 3: ditherPattern = (uint8_t*)DITHER_PATTERN_MID; intensity = 50; break;
            case 4: ditherPattern = (uint8_t*)DITHER_PATTERN_HIGH; intensity = 10; break;
            case 5: ditherPattern = (uint8_t*)DITHER_PATTERN_HIGH; intensity = 100; break;
        }
        
        //uint8_t depth = calculateDepth((vert.x.y + vert.y.y + vert.z.y) / 3); // Average depth
        
        //int bx = vert.x.x / BLOCK_SIZE;
        //int by = vert.x.y / BLOCK_SIZE;
        //int depthIndex = by * DEPTH_WIDTH + bx;
        
        // Store the new closest depth
        //depthBuffer[depthIndex] = depth;
        
        
        switch (frameBuffer.registery[0x0A]) {
            
        case 0: // Draw a shaded triangle
            drawShadedTriangle3D(vert.x.x, vert.x.y, vert.x.z, 
                                vert.y.x, vert.y.y, vert.y.z, 
                                vert.z.x, vert.z.y, vert.z.z, 
                                frameBuffer.uniforms.floats[0], // Translation
                                frameBuffer.uniforms.floats[1], 
                                frameBuffer.uniforms.floats[2], 
                                frameBuffer.uniforms.floats[3], // Rotation
                                frameBuffer.uniforms.floats[4], 
                                frameBuffer.uniforms.floats[5], 
                                frameBuffer.uniforms.floats[6], 
                                intensity, ditherPattern, 2);
            break;
            
        case 1: // Draw lines
            if (frameBuffer.registery[0x0A] == 1) {
                drawTriangle3D(vert.x.x, vert.x.y, vert.x.z, 
                            vert.y.x, vert.y.y, vert.y.z, 
                            vert.z.x, vert.z.y, vert.z.z, 
                            frameBuffer.uniforms.floats[0], // Translation
                            frameBuffer.uniforms.floats[1], 
                            frameBuffer.uniforms.floats[2], 
                            frameBuffer.uniforms.floats[3], // Rotation
                            frameBuffer.uniforms.floats[4], 
                            frameBuffer.uniforms.floats[5], 
                            frameBuffer.uniforms.floats[6]);
            }
            break;
            
        }
        
        continue;
    }
    sei();
    return;
}


void renderDrawGlyph(uint8_t* glyph, uint16_t width, uint16_t char_offset) {
    
    uint8_t halfSelectWriteBegin = 0;
    uint8_t halfSelectWriteEnd = 0;
    uint8_t halfSelectOffset = 0;
    
    // Set proper display half
    if (cursor_position > 63) {
        halfSelectWriteBegin = 0b00001101;  // Right half
        halfSelectWriteEnd   = 0b00001100;
        halfSelectOffset = cursor_position - 64;
    } else {
        halfSelectWriteBegin = 0b00010101; // Left half
        halfSelectWriteEnd   = 0b00010100;
        halfSelectOffset = cursor_position;
    }
    
    
    
    for (uint16_t i = 0; i < width; i++) {
        // Set the glyph slice
        PORTA = glyph[i + (width * char_offset)];
        
        // Trigger the write to the display
        PORTD = halfSelectWriteBegin;
        PORTD = halfSelectWriteEnd;
        
        setPosition(halfSelectOffset);
        cursor_position++;
    }
    
    return;
}


void renderDrawGlyphText(uint16_t width, uint16_t char_offset) {
    
    for (uint16_t i = 0; i < width; i++) {
        uint8_t glyphSlice = pgm_read_byte(&char_rom[i + (6 * char_offset)]);
        
        PORTA = glyphSlice;
        
        // Display half selects
        if (cursor_position > 63) {
            PORTD = 0b00001101;
            PORTD = 0b00001100;
            _delay_us(10);
            PORTD = 0b00000101;
        } else {
            PORTD = 0b00010101;
            PORTD = 0b00010100;
            _delay_us(10);
            PORTD = 0b00000101;
        }
        
        cursor_position++;
    }
}



void swapBuffers(void) {
    memcpy(frameBuffer.vram, frameBuffer.back, 1024);
    return;
}

