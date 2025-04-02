#pragma once 
#include <stdint.h> 
#include <stdio.h> 
#include <stdlib.h>

// Define the constants with a ZOBRIST_ prefix. 
static const uint64_t ZOBRIST_SEED = 123123123ULL; 
static const int ZOBRIST_RED_TO_MOVE = 84; 
static const int ZOBRIST_YELLOW_TO_MOVE = 85; 
static const int ZOBRIST_YELLOW_OFFSET = 42;

// Structure to hold the PRNG state. 
typedef struct { uint64_t a; uint64_t b; uint64_t c; uint64_t d; } RanCtx;

static inline uint64_t Rot64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }

static inline uint64_t RanVal(RanCtx *x) { uint64_t e = x->a - Rot64(x->b, 7); x->a = x->b ^ Rot64(x->c, 13); x->b = x->c + Rot64(x->d, 37); x->c = x->d + e; x->d = e + x->a; return x->d; }

static inline void RanInit(RanCtx *x, uint64_t seed) { x->a = 0xf1ea5eed; x->b = seed; x->c = seed; x->d = seed; for (int i = 0; i < 20; i++) { RanVal(x); } }

static inline void zobristInit(uint64_t x[86]) { RanCtx ctx; RanInit(&ctx, ZOBRIST_SEED); for (int i = 0; i < 86; i++) { x[i] = RanVal(&ctx); } }