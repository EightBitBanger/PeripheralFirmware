#ifndef _DITHER_EFFECTS__
#define _DITHER_EFFECTS__

static const uint8_t DITHER_PATTERN_VERT_LINES[2][2] = {{0, 2}, {1, 3}};
static const uint8_t DITHER_PATTERN_HORZ_LINES[2][2] = {{0, 1}, {2, 3}};

static const uint8_t DITHER_PATTERN_LOW[2][2] = {
    {0, 3},
    {3, 3},
};

static const uint8_t DITHER_PATTERN_MID[2][2] = {
    {3, 0},
    {0, 3},
};


static const uint8_t DITHER_PATTERN_HIGH[2][2] = {
    {0, 1},
    {0, 0},
};


#endif
