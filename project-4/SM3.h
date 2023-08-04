#pragma once
#ifndef SM3_SM3_H
#define SM3_SM3_H

#include <cstdint>
#include <vector>
#include <immintrin.h>
#include <xmmintrin.h>

typedef uint32_t Word;
typedef uint8_t Byte;
typedef __m256i Word_8;

static Word state[8];

class SM3 {
private:
	static void CF(Word* V, Byte* B);
	static Word P0(Word X);
	static Word P1(Word X);
	static Word FF(int j, Word X, Word Y, Word Z);
	static Word GG(int j, Word X, Word Y, Word Z);
	static Word T(int j);
public:
	static Word* hash(Byte* input, uint64_t inputLen);
};

class SM3_SIMD {
private:
	static void CF_SIMD(Word* V, Byte* B);
	static Word P0_SIMD(Word X);
	static Word P1_SIMD(Word X);
	static Word FF_SIMD(int j, Word X, Word Y, Word Z);
	static Word GG_SIMD(int j, Word X, Word Y, Word Z);
	static Word T_SIMD(int j);
public:
	static Word* hash_SIMD(Byte* input, uint64_t inputLen);
};

#endif //SM3_SM3_H