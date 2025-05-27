// 2D primitive shape drawing functions
//

static inline void drawPixel(uint8_t x, uint8_t y);
static inline void clearPixel(uint8_t x, uint8_t y);

static inline void drawLine(int x0, int y0, int x1, int y1);
void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void drawQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void drawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void drawCircle(int xc, int yc, int r);
void drawFilledCircle(int xc, int yc, int r);
void drawRotatedRect(int xc, int yc, int w, int h, float angle);


static inline void drawPixel(uint8_t x, uint8_t y) {
    frameBuffer.back[ (y / 8) * 128 + x ] |= (1 << (y % 8));
    return;
}


static inline void clearPixel(uint8_t x, uint8_t y) {
    frameBuffer.back[ (y / 8) * 128 + x ] &= ~(1 << (y % 8));
    return;
}

static inline void drawLine(int x0, int y0, int x1, int y1) {
    
    if (x0 > 127) x0 = 127; else if (x0 < 0) x0 = 0;
    if (x1 > 127) x1 = 127; else if (x1 < 0) x1 = 0;
    if (y0 > 63) y0 = 64; else if (y0 < 0) y0 = 0;
    if (y1 > 63) y1 = 64; else if (y1 < 0) y1 = 0;
    
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        drawPixel(x0, y0);  
        
        if (x0 == x1 && y0 == y1) break;
        int e2 = err * 2;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
        /*
        drawPixel(x0, y0);
        
        if (x0 == x1 && y0 == y1) return;  // Avoid unnecessary extra check
        
        int e2 = err << 1;  // Faster than multiplication
        if (e2 > -dy) { err -= dy; x0 += sx; }
        else if (e2 < dx) { err += dx; y0 += sy; }
        */
    }
}

void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    drawLine(x, y, x + w, y);         // Top
    drawLine(x, y + h, x + w, y + h); // Bottom
    drawLine(x, y, x, y + h);         // Left
    drawLine(x + w, y, x + w, y + h); // Right
    return;
}

void drawQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    drawLine(x1, y1, x2, y2);  // Top edge
    drawLine(x2, y2, x3, y3);  // Right edge
    drawLine(x3, y3, x4, y4);  // Bottom edge
    drawLine(x4, y4, x1, y1);  // Left edge
    return;
}

void drawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    for (uint8_t i = 0; i < h; i++) 
        drawLine(x, y + i, x + w, y + i);
    return;
}

void drawCircle(int xc, int yc, int r) {
    int x = r, y = 0;
    int p = 1 - r;  // Initial decision parameter
    
    while (x >= y) {
        drawPixel(xc + x, yc + y);
        drawPixel(xc - x, yc + y);
        drawPixel(xc + x, yc - y);
        drawPixel(xc - x, yc - y);
        drawPixel(xc + y, yc + x);
        drawPixel(xc - y, yc + x);
        drawPixel(xc + y, yc - x);
        drawPixel(xc - y, yc - x);
        
        y++;
        if (p <= 0) {
            p += 2 * y + 1;
        } else {
            x--;
            p += 2 * (y - x) + 1;
        }
    }
    return;
}

void drawFilledCircle(int xc, int yc, int r) {
    int x = r, y = 0;
    int p = 1 - r;
    
    while (x >= y) {
        // Draw horizontal scan lines to fill the circle
        drawLine(xc - x, yc + y, xc + x, yc + y);
        drawLine(xc - x, yc - y, xc + x, yc - y);
        drawLine(xc - y, yc + x, xc + y, yc + x);
        drawLine(xc - y, yc - x, xc + y, yc - x);
        
        y++;
        if (p <= 0) {
            p += 2 * y + 1;
        } else {
            x--;
            p += 2 * (y - x) + 1;
        }
    }
    return;
}


void drawRotatedRect(int xc, int yc, int w, int h, float angle) {
    float rad = angle * (3.14159265359 / 180); // Convert degrees to radians
    
    // Compute rotated corner positions
    int x0 = xc + (-w / 2) * cos(rad) - (-h / 2) * sin(rad);
    int y0 = yc + (-w / 2) * sin(rad) + (-h / 2) * cos(rad);
    
    int x1 = xc + (w / 2) * cos(rad) - (-h / 2) * sin(rad);
    int y1 = yc + (w / 2) * sin(rad) + (-h / 2) * cos(rad);
    
    int x2 = xc + (w / 2) * cos(rad) - (h / 2) * sin(rad);
    int y2 = yc + (w / 2) * sin(rad) + (h / 2) * cos(rad);
    
    int x3 = xc + (-w / 2) * cos(rad) - (h / 2) * sin(rad);
    int y3 = yc + (-w / 2) * sin(rad) + (h / 2) * cos(rad);
    
    // Connect the four rotated points
    drawLine(x0, y0, x1, y1);
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x0, y0);
    
    return;
}

