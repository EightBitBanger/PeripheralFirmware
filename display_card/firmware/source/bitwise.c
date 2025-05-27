#include "bitwise.h"

uint8_t FlipBits(uint8_t input) {
    
    uint8_t output = 0;
    
    if (input & 0b00000001) output += (1 << 7);
    if (input & 0b00000010) output += (1 << 6);
    if (input & 0b00000100) output += (1 << 5);
    if (input & 0b00001000) output += (1 << 4);
    if (input & 0b00010000) output += (1 << 3);
    if (input & 0b00100000) output += (1 << 2);
    if (input & 0b01000000) output += (1 << 1);
    if (input & 0b10000000) output += (1 << 0);
    
    return output;
}


uint8_t GetBit(uint8_t input, uint8_t bit) {
    
    if (bit == 0) {if (input & 0b00000001) return 1;}
    if (bit == 1) {if (input & 0b00000010) return 1;}
    if (bit == 2) {if (input & 0b00000100) return 1;}
    if (bit == 3) {if (input & 0b00001000) return 1;}
    if (bit == 4) {if (input & 0b00010000) return 1;}
    if (bit == 5) {if (input & 0b00100000) return 1;}
    if (bit == 6) {if (input & 0b01000000) return 1;}
    if (bit == 7) {if (input & 0b10000000) return 1;}
    
    return 0;
}


uint8_t InvertBits(uint8_t input) {
    
    uint8_t output = 0;
    
    if (GetBit(input, 0) != 1) {output += (1 << 0);}
    if (GetBit(input, 1) != 1) {output += (1 << 1);}
    if (GetBit(input, 2) != 1) {output += (1 << 2);}
    if (GetBit(input, 3) != 1) {output += (1 << 3);}
    if (GetBit(input, 4) != 1) {output += (1 << 4);}
    if (GetBit(input, 5) != 1) {output += (1 << 5);}
    if (GetBit(input, 6) != 1) {output += (1 << 6);}
    if (GetBit(input, 7) != 1) {output += (1 << 7);}
    
    return output;
}

void ReorientBits(uint8_t* byteArray) {
    
    uint8_t tempArray[8];
    
    for (uint8_t i=0; i < 8; i++) 
        tempArray[i] = byteArray[i];
    
    if (GetBit(tempArray[0], 0) == 0) {byteArray[0] = 0;} else {byteArray[0] = 1;}
    if (GetBit(tempArray[0], 1) == 0) {byteArray[1] = 0;} else {byteArray[1] = 1;}
    if (GetBit(tempArray[0], 2) == 0) {byteArray[2] = 0;} else {byteArray[2] = 1;}
    if (GetBit(tempArray[0], 3) == 0) {byteArray[3] = 0;} else {byteArray[3] = 1;}
    if (GetBit(tempArray[0], 4) == 0) {byteArray[4] = 0;} else {byteArray[4] = 1;}
    if (GetBit(tempArray[0], 5) == 0) {byteArray[5] = 0;} else {byteArray[5] = 1;}
    if (GetBit(tempArray[0], 6) == 0) {byteArray[6] = 0;} else {byteArray[6] = 1;}
    if (GetBit(tempArray[0], 7) == 0) {byteArray[7] = 0;} else {byteArray[7] = 1;}
    
    return;
}








