// =================================
//   The frame buffer memory layout which describes different settings and buffers to which
//  data can be written and read. The first few bytes 0x00 - 0x10 are used for the following 
//  device configuration settings.
//  
//  
//  0x00 Video modes  0=TEXT  1=GRAPHICS  2=SPRITE  3=VERTEX
//      Set the rendering mode.
//  
//  0x01 Cursor line
//      Cursor line 0-8.
//  
//  0x02 Cursor position
//      Should be between 0-21 for text and 0-16 for sprites.
//      Graphics mode is between 0-128.
//  
//  0x03 Cursor blink rate
//      Rate at which the cursor will blink.
//  
//  0x04 Cursor character
//      Character used as the cursor.
//  
//  0x05 Shift screen up
//      Flag to cause the VRAM to get shifted up. It will take 
//      a few cycles to complete this operation.
//  
//  0x06 Invert display output
//      Inverts the bits in the glyph when they are rendered.
//  
//  0x07 Select memory region  0=VRAM  1=CHAR_RAM  2=SPRITE_SHEET  3=VERTEX_BUFFER  4=UNIFORM_VARIABLES
//      Selects the memory region to access. This can be useful for writing to 
//      specific memory buffers.
//  
//  0x08 Vertex buffer size
//      The number of vertices in the buffer.
//  
//  0x09 Flag to clear the back frame buffer
//      This will set the back frame buffer to all zeros.
//  
//  0x0A Flag to set the primitive drawing type
//      Select from lines or triangles.
//  
//  0x0B 
//  
//  0x0C Begin drawing
//      Vertex index to begin drawing.
//  
//  0x0D vertices to draw
//      Number of vertices to render.
//  
//  0x0E Draw vertex range
//      Draw the range of vertices in the buffer starting at the value set in 
//      register 0x0C and the number to draw at register 0x0D
//  
//  0x0F Swap frame buffers
//      Trigger a swap of the frame buffers.
//  
//  
//      The next 1024 bytes are used to access the memory block selected by setting the 
//     above register at '0x07'.
//  
//  0x10 Video memory
//      Beginning of the memory used for video generation.
//  
//  


#ifndef _MAIN_FUNC__
#define _MAIN_FUNC__

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 20000000UL
#include <util/delay.h>

#define DisableGlobalInterrupts   __asm__("cli")
#define EnableGlobalInterrupts     __asm__("sei")

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BLOCK_SIZE 8

#define DEPTH_WIDTH (SCREEN_WIDTH / BLOCK_SIZE)  // 16
#define DEPTH_HEIGHT (SCREEN_HEIGHT / BLOCK_SIZE) // 8


typedef struct { int x, y, z; } Point3;
typedef struct { int x, y; } Point2;
typedef struct { Point2 x, y; } Line;

typedef struct { Point3 x, y, z; } Vertex;

union uniform {
    float floats[8];
    
    uint8_t bytes[32];
};

struct FrameBuffer {
    
    volatile uint8_t registery[0x10];  // Device registers
    
    uint8_t vram[1024];                // Video memory
    uint8_t back[1024];                // Swap buffer
    
    union uniform uniforms;
};

#endif
