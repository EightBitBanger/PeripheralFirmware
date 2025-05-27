#ifndef _VERTEX_BUFFER_MEMORY__
#define _VERTEX_BUFFER_MEMORY__

struct Vertex {
    Point2 x;
    Point2 y;
    Point2 z;
    
    int8_t dither;
};


int8_t vertex_buffer[512];

#endif
