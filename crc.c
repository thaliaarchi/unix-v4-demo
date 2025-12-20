#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint16_t parity(uint8_t x)
{
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return (x & 1) ^ 1;
}

// Add parity bit to data.
void add_parity(const uint8_t *in, uint16_t *out, size_t len)
{
	for (size_t i = 0; i < len; i++)
		out[i] = ((uint16_t)in[i] << 1) | parity(in[i]);
}

// Print CRC and LRC.
void print_check(const uint16_t *data, size_t len)
{
	uint16_t crc = 0;
	uint16_t lrc = 0;

	// CRC and LRC calculation code from readtape.
	for (size_t i = 0; i < len; i++) {
		lrc ^= data[i];
		crc ^= data[i]; // C0..C7,P  (See IBM Form A22-6862-4)
		if (crc & 2) {
			crc ^= 0xf0; // if P will become 1 after rotate, invert what will go into C2..C5
		}
		uint16_t lsb = crc & 1; // rotate all 9 bits
		crc >>= 1;
		if (lsb) {
			crc |= 0x100;
		}
	}
	crc ^= 0x1af; // invert all except C2 and C4; note that the CRC could be zero if the number of data bytes is odd
	lrc ^= crc;  // LRC inlcudes the CRC (the manual doesn't say that!)

	printf("CRC=%d\n", crc);
	printf("LRC=%d\n", lrc);
}

int main(int argc, char **argv)
{
	FILE *fp = fopen(argv[1], "rb");
	if (!fp)
		return 1;
	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	uint8_t *data = (uint8_t *)malloc(len);
	uint16_t *withParity = (uint16_t *)malloc(len * sizeof(uint16_t));
	fseek(fp, 0, SEEK_SET);
	fread(data, 1, len, fp);
	fclose(fp);
	add_parity(data, withParity, len);
	free(data);
	print_check(withParity, len);
	free(withParity);
	return 0;
}
