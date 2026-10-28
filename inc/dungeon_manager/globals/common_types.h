#pragma once

#include <stdint.h>

typedef struct{
    int16_t x;
    int16_t y;
}vector2_t;

static inline void vector2_add(vector2_t *dst, const vector2_t *src) {
    dst->x += src->x;
    dst->y += src->y;
}

static inline void vector2_sub(vector2_t *dst, const vector2_t *src) {
    dst->x -= src->x;
    dst->y -= src->y;
}

static inline vector2_t vector2_add_new(vector2_t a, vector2_t b) {
    return (vector2_t){ a.x + b.x, a.y + b.y };
}

static inline vector2_t vector2_sub_new(vector2_t a, vector2_t b) {
    return (vector2_t){ a.x - b.x, a.y - b.y };
}
