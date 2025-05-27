// 3D drawing and rasterization functions
//

void drawLine3D(int x1, int y1, int x2, int y2, int xx, int yy, float angleX, float angleY, float angleZ);
void drawTriangle3D(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, 
                    float xx, float yy, float zz, float angleX, float angleY, float angleZ, float scale);
void drawShadedTriangle3D(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, 
                          float xx, float yy, float zz, float angleX, float angleY, float angleZ, float scale, int intensity, 
                          const uint8_t* ditherPattern, int patternSize);


// Low level rendering functions
void projectPoint(Point3 p, int* x, int* y);
uint8_t shouldDrawPixel(int x, int y, int intensity, const uint8_t* ditherPattern, int patternSize);

void rotateX(Point3* p, float angle);
void rotateY(Point3* p, float angle);
void rotateZ(Point3* p, float angle);



void drawLine3D(int x1, int y1, int x2, int y2, int xx, int yy, float angleX, float angleY, float angleZ) {
    // Define the line by the two points
    Point3 lineVertices[2] = {
        {x1, y1, 0}, {x2, y2, 0}  // Start point (x1, y1), end point (x2, y2)
    };
    
    Point3 transformedVertices[2];
    int x[2], y[2];
    
    // Apply transformations
    for (int i = 0; i < 2; i++) {
        transformedVertices[i] = lineVertices[i];
        
        // Rotate
        rotateX(&transformedVertices[i], angleX);
        rotateY(&transformedVertices[i], angleY);
        rotateZ(&transformedVertices[i], angleZ);
        
        // Translate
        transformedVertices[i].x += xx;
        transformedVertices[i].y += yy;
        
        projectPoint(transformedVertices[i], &x[i], &y[i]);
    }
    
    // Draw the line between the two transformed points
    drawLine(x[0], y[0], x[1], y[1]);
    return;
}



void drawTriangle3D(int x1, int y1, int z1, 
                    int x2, int y2, int z2, 
                    int x3, int y3, int z3, 
                    float xx, float yy, float zz, 
                    float angleX, float angleY, float angleZ, 
                    float scale) {
    // Transform triangle vertices
    Point3 triangleVertices[3] = {
        {x1 * 8, y1 * 8, z1 * 8},
        {x2 * 8, y2 * 8, z2 * 8},
        {x3 * 8, y3 * 8, z3 * 8}
    };
    
    Point3 transformedVertices[3];
    int x[3], y[3];
    
    for (int i = 0; i < 3; i++) {
        transformedVertices[i] = triangleVertices[i];
        
        // Apply transformations
        rotateX(&transformedVertices[i], angleX);
        rotateY(&transformedVertices[i], angleY);
        rotateZ(&transformedVertices[i], angleZ);
        
        // Translate
        transformedVertices[i].x += xx;
        transformedVertices[i].y += yy;
        transformedVertices[i].z += zz;
        
        // Project to 2D
        projectPoint(transformedVertices[i], &x[i], &y[i]);
    }
    
    // Draw the edges of the triangle
    drawLine(x[0], y[0], x[1], y[1]);
    drawLine(x[1], y[1], x[2], y[2]);
    drawLine(x[2], y[2], x[0], y[0]);
    return;
}

void drawShadedTriangle3D(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, 
                          float xx, float yy, float zz, float angleX, float angleY, float angleZ, float scale, int intensity, 
                          const uint8_t* ditherPattern, int patternSize) {
    // Transform triangle vertices
    int scaleInt = (int)scale;
    Point3 triangleVertices[3] = {
        {x1 * scaleInt, y1 * scaleInt, z1 * scaleInt},
        {x2 * scaleInt, y2 * scaleInt, z2 * scaleInt},
        {x3 * scaleInt, y3 * scaleInt, z3 * scaleInt}
    };
    
    Point3 transformedVertices[3];
    int x[3], y[3];
    
    for (int i = 0; i < 3; i++) {
        transformedVertices[i] = triangleVertices[i];
        
        // Apply transformations
        rotateX(&transformedVertices[i], angleX);
        rotateY(&transformedVertices[i], angleY);
        rotateZ(&transformedVertices[i], angleZ);
        
        // Translate
        transformedVertices[i].x += xx;
        transformedVertices[i].y += yy;
        transformedVertices[i].z += zz;
        
        // Project to 2D
        projectPoint(transformedVertices[i], &x[i], &y[i]);
    }
    
    
    // Sort vertices by Y-coordinate (y0 < y1 < y2)
    if (y[0] > y[1]) { 
        int temp; 
        temp = x[0]; x[0] = x[1]; x[1] = temp;
        temp = y[0]; y[0] = y[1]; y[1] = temp;
    }
    if (y[0] > y[2]) { 
        int temp; 
        temp = x[0]; x[0] = x[2]; x[2] = temp;
        temp = y[0]; y[0] = y[2]; y[2] = temp;
    }
    if (y[1] > y[2]) { 
        int temp; 
        temp = x[1]; x[1] = x[2]; x[2] = temp;
        temp = y[1]; y[1] = y[2]; y[2] = temp;
    }
    
    // Compute edge slopes
    int dx01 = x[1] - x[0], dy01 = y[1] - y[0];
    int dx02 = x[2] - x[0], dy02 = y[2] - y[0];
    int dx12 = x[2] - x[1], dy12 = y[2] - y[1];
    
    float invSlope01 = (dy01 != 0) ? (float)dx01 / dy01 : 0;
    float invSlope02 = (dy02 != 0) ? (float)dx02 / dy02 : 0;
    float invSlope12 = (dy12 != 0) ? (float)dx12 / dy12 : 0;
    
    // Rasterize triangle in two parts (top and bottom)
    int yCur;
    for (yCur = y[0]; yCur <= y[2]; yCur++) {
        int lowerHalf = (yCur > y[1]);
        float xStart = x[0] + (yCur - y[0]) * invSlope02;
        float xEnd = lowerHalf ? x[1] + (yCur - y[1]) * invSlope12 : x[0] + (yCur - y[0]) * invSlope01;
        
        if (xStart > xEnd) {
            float temp = xStart;
            xStart = xEnd;
            xEnd = temp;
        }
        
        // Rasterize the scanline
        int xCur;
        for (xCur = (int)xStart; xCur <= (int)xEnd; xCur++) {
            if (shouldDrawPixel(xCur, yCur, intensity, ditherPattern, patternSize)) {
                drawPixel(xCur, yCur);
            }
        }
    }
    
    return;
}











// Low level rendering functions
//

void rotateX(Point3* p, float angle) {
    float rad = angle * (3.14159265359f / 180.0f);
    int y = p->y * cos(rad) - p->z * sin(rad);
    int z = p->y * sin(rad) + p->z * cos(rad);
    p->y = y;
    p->z = z;
}

void rotateY(Point3* p, float angle) {
    float rad = angle * (3.14159265359f / 180.0f);
    int x =  p->x * cos(rad) + p->z * sin(rad);
    int z = -p->x * sin(rad) + p->z * cos(rad);
    p->x = x;
    p->z = z;
}

void rotateZ(Point3* p, float angle) {
    float rad = angle * (3.14159265359f / 180.0f);
    int x = p->x * cos(rad) - p->y * sin(rad);
    int y = p->x * sin(rad) + p->y * cos(rad);
    p->x = x;
    p->y = y;
}

// Project to 2D display space
void projectPoint(Point3 p, int* x, int* y) {
    int scale = 32.0f;
    int depth = 200.0f;
    
    *x = (p.x * scale) / (p.z + depth) + 64;
    *y = (p.y * scale) / (p.z + depth) + 32;
}

// Function to determine if a pixel should be drawn based on a given dithering pattern
uint8_t shouldDrawPixel(int x, int y, int intensity, const uint8_t* ditherPattern, int patternSize) {
    int threshold = ditherPattern[(y % patternSize) * patternSize + (x % patternSize)] * (255 / (patternSize * patternSize));
    return (intensity > threshold) ? 1 : 0;  // Draw if intensity is above the threshold
}

