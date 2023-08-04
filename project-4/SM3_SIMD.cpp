#include "SM3.h"
#include <immintrin.h>
#include <xmmintrin.h>

#define ROTATELEFT(X, n)  (((X)<<(n)) | ((X)>>(32-(n))))

using namespace std;

Word* SM3::hash(Byte* input, uint64_t inputLen) {
	Word_8 VI = _mm256_setr_epi32(0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e);
	Word* state = new Word[8];
	memcpy(state, VI, sizeof(VI));
	Byte buf[64];
	int inputPtr = 0;
	int bufPtr = 0;

	while (inputPtr < inputLen) {
		buf[bufPtr] = input[inputPtr];
		inputPtr++;
		bufPtr++;

		if (bufPtr == 64) {
			CF(state, buf);
			bufPtr = 0;
		}
	}

	buf[bufPtr++] = 0x80;
	if (64 - bufPtr < 8) {
		while (bufPtr < 64) buf[bufPtr++] = 0;
		bufPtr = 0;
		CF(state, buf);
	}
	while (bufPtr < 56) buf[bufPtr++] = 0;

	inputLen *= 8;
	buf[63] = static_cast<Byte>(inputLen & 0x00000000000000ff);
	buf[62] = static_cast<Byte>((inputLen & 0x000000000000ff00) >> 8);
	buf[61] = static_cast<Byte>((inputLen & 0x0000000000ff0000) >> 16);
	buf[60] = static_cast<Byte>((inputLen & 0x00000000ff000000) >> 24);
	buf[59] = static_cast<Byte>((inputLen & 0x000000ff00000000) >> 32);
	buf[58] = static_cast<Byte>((inputLen & 0x0000ff0000000000) >> 40);
	buf[57] = static_cast<Byte>((inputLen & 0x00ff000000000000) >> 48);
	buf[56] = static_cast<Byte>((inputLen & 0xff00000000000000) >> 56);

	CF(state, buf);
	return state;
}

__m128i left(__m128i a, int k)
{
	k = k % 32;
	__m128i tmp1, tmp2, tmp3, tmp4;
	__m128i ze = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	tmp2 = _mm_slli_epi32(a, k);
	tmp2 = _mm_and_si128(ze, tmp2);
	tmp3 = _mm_srli_epi32(_mm_and_si128(ze, a), 32 - k);
	tmp4 = _mm_or_si128(tmp2, tmp3);
	return tmp4;
}

void SM3::CF(Word* V, Byte* Bi) {
	auto W = std::vector<int>(68, 0); // W
	auto WW = std::vector<Word>(64, 0); // W'
	__m128i temp[17];
	__m128i num1, num2, num3, num4, num5, num6, num7, num8, num9;
	__m128i ze = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	for (int i = 0; i < 16; ++i) {
		W[i] = 0;
		W[i] |= ((Word)Bi[i * 4] << 24);
		W[i] |= ((Word)Bi[i * 4 + 1] << 16);
		W[i] |= ((Word)Bi[i * 4 + 2] << 8);
		W[i] |= ((Word)Bi[i * 4 + 3]);
	}
	for (int j = 4; j < 17; j++)//基于SIMD对消息扩展进行优化
	{
		int i = j * 4 + 3;
		num1 = _mm_setr_epi32(W[j * 4 - 16], W[j * 4 - 15], W[j * 4 - 14], W[j * 4 - 13]);
		num2 = _mm_setr_epi32(W[j * 4 - 13], W[j * 4 - 12], W[j * 4 - 11], W[j * 4 - 10]);
		num3 = _mm_setr_epi32(W[j * 4 - 9], W[j * 4 - 8], W[j * 4 - 7], W[j * 4 - 6]);
		num4 = _mm_setr_epi32(W[j * 4 - 3], W[j * 4 - 2], W[j * 4 - 1], 0);
		num5 = _mm_setr_epi32(W[j * 4 - 6], W[j * 4 - 5], W[j * 4 - 4], W[j * 4 - 3]);
		num6 = _mm_xor_si128(num1, num3);
		num7 = left(num4, 15);
		num6 = _mm_xor_si128(num6, num7);
		num8 = _mm_xor_si128(num6, _mm_xor_si128(left(num6, 15), left(num6, 23)));
		num9 = _mm_xor_si128(left(num2, 7), num5);
		temp[j] = _mm_xor_si128(num8, num9);
		_mm_maskstore_epi32(&W[j * 4], ze, temp[j]);
		W[i] = P1(W[i - 16] ^ W[i - 9] ^ (ROTATELEFT(W[i - 3], 15))) ^ (ROTATELEFT(W[i - 13], 7)) ^ W[i - 6];
	}
	for (int i = 0; i <= 63; ++i) {
		WW[i] = W[i] ^ W[i + 4];
	}
	constexpr int A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7;
	Word reg[8];

	for (int j = 0; j < 8; ++j) {
		reg[j] = V[j];
	}

	for (int j = 0; j <= 63; ++j) {
		Word SS1, SS2, TT1, TT2;
		SS1 = ROTATELEFT(ROTATELEFT(reg[A], 12) + reg[E] + ROTATELEFT(T(j), j), 7);
		SS2 = SS1 ^ ROTATELEFT(reg[A], 12);
		TT1 = FF(j, reg[A], reg[B], reg[C]) + reg[D] + SS2 + WW[j];
		TT2 = GG(j, reg[E], reg[F], reg[G]) + reg[H] + SS1 + W[j];
		reg[D] = reg[C];
		reg[C] = ROTATELEFT(reg[B], 9);
		reg[B] = reg[A];
		reg[A] = TT1;
		reg[H] = reg[G];
		reg[G] = ROTATELEFT(reg[F], 19);
		reg[F] = reg[E];
		reg[E] = P0(TT2);

	}
	for (int i = 0; i < 8; ++i) {
		V[i] ^= reg[i];
	}
}

Word SM3::P0(Word X) {
	return X ^ ROTATELEFT(X, 9) ^ ROTATELEFT(X, 17);
}

Word SM3::P1(Word X) {
	return X ^ ROTATELEFT(X, 15) ^ ROTATELEFT(X, 23);
}

Word SM3::T(int j) {
	if (j <= 15) {
		return 0x79cc4519;
	}
	else {
		return 0x7a879d8a;
	}
}

Word SM3::FF(int j, Word X, Word Y, Word Z) {
	if (j <= 15) {
		return X ^ Y ^ Z;
	}
	else {
		return (X & Y) | (X & Z) | (Y & Z);
	}
}

Word SM3::GG(int j, Word X, Word Y, Word Z) {
	if (j <= 15) {
		return X ^ Y ^ Z;
	}
	else {
		return (X & Y) | ((~X) & Z);
	}
}