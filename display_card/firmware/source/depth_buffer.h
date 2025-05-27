// Block depth buffering
//

uint8_t depthBuffer[DEPTH_WIDTH * DEPTH_HEIGHT]; // 128 bytes

void clearDepthBuffer() {
    memset(depthBuffer, 255, sizeof(depthBuffer)); // 255 = farthest
}

void drawPixelDepth(int x, int y, uint8_t depth) {
    int bx = x / BLOCK_SIZE;
    int by = y / BLOCK_SIZE;
    int index = by * DEPTH_WIDTH + bx;

    if (depth < depthBuffer[index]) { // Closer object updates block depth
        depthBuffer[index] = depth;
        drawPixel(x, y);
    }
}

#define Z_NEAR 10   // Closest depth value
#define Z_FAR 1000  // Farthest depth value

uint8_t calculateDepth(int z) {
    if (z < Z_NEAR) return 0;    // Clamp to near
    if (z > Z_FAR) return 255;   // Clamp to far
    
    return (uint8_t)(((z - Z_NEAR) * 255) / (Z_FAR - Z_NEAR));
}

