#include <stdio.h>
#include <stdlib.h>

#include "png.h"

// A precomputed 32-bit CRC (Cyclic Redundancy Code) lookup table, used to speed
// up the creation of a CRC checksum for each chunk in a PNG file. More
// information about how to get the standard 32-bit CRC values may be found at
// http://www.barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code.
const uint32_t crc32_lookup_table[] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
	0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
	0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
	0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
	0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
	0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
	0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
	0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
	0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
	0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
	0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
	0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
	0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
	0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
	0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
	0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
	0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
	0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
	0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
	0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
	0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
	0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

// The idea of optimizing the Adler-32 checksum by delaying the modulo
// operation for MAX_ADDITIONS is accredited to Mark Adler. His Adler-32 code
// may be found at https://github.com/madler/zlib/blob/master/adler32.c.

#define BASE 65521 // largest prime number smaller than 2^16

// largest x such that 255x(x+1)/2 + (x+1)(BASE-1) <= 2^32-1
#define MAX_ADDITIONS 5552

// updates the two parts of an Adler-32 checksum with the given character
#define ADLER(character, sum1, sum2) {(sum1) += (character); (sum2) += (sum1);}

// applies the modulo operation to the two parts of an Adler-32 checksum
#define MOD_SUMS(sum1, sum2) {(sum1) %= BASE; (sum2) %= BASE;}

/* ========================================================================== */

#define IHDR 0x49484452 // hex for "IHDR"
#define IHDR_CRC 0x575E51F5 // CRC checksum of the string "IHDR"

#define IDAT 0x49444154 // hex for "IDAT"
#define IDAT_CRC 0xCA50F9E1 // CRC checksum of the string "IDAT"

#define IEND 0x49454E44 // hex for "IEND"
#define IEND_CRC_C 0xAE426082 // 1's complement of the CRC checksum of "IEND"

// places an integer into a file stream in big-endian format
#define PUTC_INT(integer, fp) {                                                \
	putc((integer) >> 24 & 0xFF, fp);                                         \
	putc((integer) >> 16 & 0xFF, fp);                                         \
	putc((integer) >> 8 & 0xFF, fp);                                          \
	putc((integer) & 0xFF, fp);                                               \
}

// places a character into a file stream and updates the CRC checksum of that
// stream
#define PUTC_AND_CRC(character, fp, crc) {                                     \
	putc(character, fp);                                                      \
	(crc) = crc32_lookup_table[((crc) ^ (character)) & 0xFF] ^ ((crc) >> 8);  \
}

// places an integer into a file stream in big-endian format and updates the CRC
// checksum of that stream
#define PUTC_AND_CRC_INT(integer, fp, crc) {                                   \
	PUTC_AND_CRC((integer) >> 24 & 0xFF, fp, crc);                            \
	PUTC_AND_CRC((integer) >> 16 & 0xFF, fp, crc);                            \
	PUTC_AND_CRC((integer) >> 8 & 0xFF, fp, crc);                             \
	PUTC_AND_CRC((integer) & 0xFF, fp, crc);                                  \
}

// A deflate block can hold at most 0xFFFF bytes, since its length is stored in
// 2 bytes.
#define MAX_BLOCK_LEN 0xFFFF

// A PNG chunk can hold at most 0xFFFFFFFF bytes, since its length is stored in
// 4 bytes.
#define MAX_CHUNK_LEN 0xFFFFFFFF

// largest x such that 6 + 5 * ceil(x / MAX_BLOCK_LEN) + x <= MAX_CHUNK_LEN
// (A PNG chunk contains the 2-byte header "\x78\x01", followed by all its data
// separated into deflate blocks, each of which has a 5-byte header, followed by
// an Adler-32 checksum of its data.)
#define MAX_DATA_LEN 0xFFFB0008

// ceil(MAX_DATA_LEN / MAX_BLOCK_LEN)
#define MAX_NUM_BLOCKS 0xFFFD

// MAX_DATA_LEN - MAX_BLOCK_LEN * (MAX_NUM_BLOCKS - 1)
#define MAX_CHUNK_LAST_BLOCK_LEN 0xFFFB

// places the header for a full deflate block into a file stream and updates the
// CRC checksum of that stream
#define BLOCK_HEADER(fp, crc) {                                                \
	PUTC_AND_CRC(0, fp, crc);                                                 \
	PUTC_AND_CRC(0xFF, fp, crc);                                              \
	PUTC_AND_CRC(0xFF, fp, crc);                                              \
	PUTC_AND_CRC(0, fp, crc);                                                 \
	PUTC_AND_CRC(0, fp, crc);                                                 \
}

// places the header for the last deflate block in a PNG data chunk into a file
// stream and updates the CRC checksum of that stream
#define LAST_BLOCK_HEADER(fp, crc, bytes_remaining) {                          \
	PUTC_AND_CRC(1, fp, crc);                                                 \
	PUTC_AND_CRC(bytes_remaining & 0xFF, fp, crc);                            \
	PUTC_AND_CRC(bytes_remaining >> 8 & 0xFF, fp, crc);                       \
	PUTC_AND_CRC((bytes_remaining ^ 0xFF) & 0xFF, fp, crc);                   \
	PUTC_AND_CRC((bytes_remaining >> 8 ^ 0xFF) & 0xFF, fp, crc);              \
}

// places the header for a last deflate block in a full PNG data chunk into a
// file stream and updates the CRC checksum of that stream
#define MAX_CHUNK_LAST_BLOCK_HEADER(fp, crc) {                                 \
	PUTC_AND_CRC(1, fp, crc);                                                 \
	PUTC_AND_CRC(0xFF, fp, crc);                                              \
	PUTC_AND_CRC(0xFB, fp, crc);                                              \
	PUTC_AND_CRC(0, fp, crc);                                                 \
	PUTC_AND_CRC(4, fp, crc);                                                 \
}

// places the first 8 bytes of a PNG file into a file stream
#define PNG_SIG(fp) {                                                          \
	putc(0x89, fp);                                                           \
	putc('P' , fp);                                                           \
	putc('N' , fp);                                                           \
	putc('G' , fp);                                                           \
	putc('\r', fp);                                                           \
	putc('\n', fp);                                                           \
	putc(0x1A, fp);                                                           \
	putc('\n', fp);                                                           \
}

/* ========================================================================== */

void header_chunk(
	uint32_t width, uint32_t height, uint8_t color_type,
	FILE *fp
	) {
	PUTC_INT(13, fp); // 2 ints and 5 chars take up 13 bytes
	PUTC_INT(IHDR, fp);

	uint32_t crc = IHDR_CRC;
	PUTC_AND_CRC_INT(width, fp, crc);
	PUTC_AND_CRC_INT(height, fp, crc);
	PUTC_AND_CRC(8, fp, crc); // 8 = 8-bit color depth
	PUTC_AND_CRC(color_type, fp, crc);
	PUTC_AND_CRC(0, fp, crc); // 0 = deflate compression
	PUTC_AND_CRC(0, fp, crc); // 0 = adaptive filtering
	PUTC_AND_CRC(0, fp, crc); // 0 = no interlace

	crc ^= 0xFFFFFFFF;
	PUTC_INT(crc, fp);
}

void data_chunks_rgb(
	uint32_t width, uint32_t height, const uint32_t *data, FILE *fp
	) {
	uint32_t max_line_pos = 3 * width;
	// each pixel consists of a red byte, a green byte, and a blue byte

	uint64_t data_len = (uint64_t)height * (max_line_pos + 1);
	// the data transfered to the file will contain an additional 0 byte at
	// the start of each line

	uint32_t line_pos = max_line_pos;
	uint32_t cur_pixel = *data++;
	uint32_t pixel_pos = 2; // 2 = red, 1 = green, 0 = blue

	uint32_t crc, adler_sum1, adler_sum2, mod_delay,
	num_blocks, block_pos, cur_byte;

	while (data_len >= MAX_DATA_LEN) {
		num_blocks = MAX_NUM_BLOCKS;

		PUTC_INT(MAX_CHUNK_LEN, fp);
		PUTC_INT(IDAT, fp);

		crc = IDAT_CRC;
		PUTC_AND_CRC(0x78, fp, crc);
		PUTC_AND_CRC(0x01, fp, crc);
		adler_sum1 = 1;
		adler_sum2 = 0;
		mod_delay = 0;

		while (--num_blocks) {
			BLOCK_HEADER(fp, crc);
			block_pos = MAX_BLOCK_LEN;
			while (block_pos--) {
				if (line_pos == max_line_pos) {
					PUTC_AND_CRC(0, fp, crc);
					adler_sum2 += adler_sum1;
					line_pos = 0;
				}
				else {
					cur_byte = cur_pixel >> 8 * pixel_pos & 0xFF;
					PUTC_AND_CRC(cur_byte, fp, crc);
					ADLER(cur_byte, adler_sum1, adler_sum2);
					line_pos++;
					if (pixel_pos-- == 0) {
						cur_pixel = *data++;
						pixel_pos = 2;
					}
				}
				if (++mod_delay == MAX_ADDITIONS) {
					MOD_SUMS(adler_sum1, adler_sum2);
					mod_delay = 0;
				}
			}
		}

		MAX_CHUNK_LAST_BLOCK_HEADER(fp, crc);
		block_pos = MAX_CHUNK_LAST_BLOCK_LEN;
		while (block_pos--) {
			if (line_pos == max_line_pos) {
				PUTC_AND_CRC(0, fp, crc);
				adler_sum2 += adler_sum1;
				line_pos = 0;
			}
			else {
				cur_byte = cur_pixel >> 8 * pixel_pos & 0xFF;
				PUTC_AND_CRC(cur_byte, fp, crc);
				ADLER(cur_byte, adler_sum1, adler_sum2);
				line_pos++;
				if (pixel_pos-- == 0) {
					cur_pixel = *data++;
					pixel_pos = 2;
				}
			}
			if (++mod_delay == MAX_ADDITIONS) {
				MOD_SUMS(adler_sum1, adler_sum2);
				mod_delay = 0;
			}
		}

		MOD_SUMS(adler_sum1, adler_sum2);

		PUTC_AND_CRC(adler_sum2 >> 8 & 0xFF, fp, crc);
		PUTC_AND_CRC(adler_sum2 & 0xFF, fp, crc);
		PUTC_AND_CRC(adler_sum1 >> 8 & 0xFF, fp, crc);
		PUTC_AND_CRC(adler_sum1 & 0xFF, fp, crc);

		crc ^= 0xFFFFFFFF;
		PUTC_INT(crc, fp);

		data_len -= MAX_DATA_LEN;
	}

	if (data_len) {
		num_blocks = 1 + ((uint32_t)data_len - 1) / MAX_BLOCK_LEN;
		// ceil(data_len / MAX_BLOCK_LEN)

		crc = 6 + 5 * num_blocks + (uint32_t)data_len;
		// (using crc to avoid making new variable)
		// 2-byte header "\x78\x01", plus a 5-byte header per deflate
		// block, plus the remaining data, plus a 4-byte Adler-32
		// checksum of the remaining data

		PUTC_INT(crc, fp);
		PUTC_INT(IDAT, fp);

		crc = IDAT_CRC;
		PUTC_AND_CRC(0x78, fp, crc);
		PUTC_AND_CRC(0x01, fp, crc);
		adler_sum1 = 1;
		adler_sum2 = 0;
		mod_delay = 0;

		while (--num_blocks) {
			BLOCK_HEADER(fp, crc);
			block_pos = MAX_BLOCK_LEN;
			while (block_pos--) {
				if (line_pos == max_line_pos) {
					PUTC_AND_CRC(0, fp, crc);
					adler_sum2 += adler_sum1;
					line_pos = 0;
				}
				else {
					cur_byte = cur_pixel >> 8 * pixel_pos & 0xFF;
					PUTC_AND_CRC(cur_byte, fp, crc);
					ADLER(cur_byte, adler_sum1, adler_sum2);
					line_pos++;
					if (pixel_pos-- == 0) {
						cur_pixel = *data++;
						pixel_pos = 2;
					}
				}
				if (++mod_delay == MAX_ADDITIONS) {
					MOD_SUMS(adler_sum1, adler_sum2);
					mod_delay = 0;
				}
			}
			data_len -= MAX_BLOCK_LEN;
		}

		LAST_BLOCK_HEADER(fp, crc, data_len);
		while (data_len--) {
			if (line_pos == max_line_pos) {
				PUTC_AND_CRC(0, fp, crc);
				adler_sum2 += adler_sum1;
				line_pos = 0;
			}
			else {
				cur_byte = cur_pixel >> 8 * pixel_pos & 0xFF;
				PUTC_AND_CRC(cur_byte, fp, crc);
				ADLER(cur_byte, adler_sum1, adler_sum2);
				line_pos++;
				if (pixel_pos-- == 0) {
					cur_pixel = *data++;
					pixel_pos = 2;
				}
			}
			if (++mod_delay == MAX_ADDITIONS) {
				MOD_SUMS(adler_sum1, adler_sum2);
				mod_delay = 0;
			}
		}

		MOD_SUMS(adler_sum1, adler_sum2);

		PUTC_AND_CRC(adler_sum2 >> 8 & 0xFF, fp, crc);
		PUTC_AND_CRC(adler_sum2 & 0xFF, fp, crc);
		PUTC_AND_CRC(adler_sum1 >> 8 & 0xFF, fp, crc);
		PUTC_AND_CRC(adler_sum1 & 0xFF, fp, crc);

		crc ^= 0xFFFFFFFF;
		PUTC_INT(crc, fp);
	}
}

void end_chunk(FILE *fp) {
	PUTC_INT(0, fp);
	PUTC_INT(IEND, fp);
	PUTC_INT(IEND_CRC_C, fp);
}

/* ========================================================================== */

void make_png(
	const char *name, const screen s, char color_type
	) {
	FILE *fp = fopen(name, "wb");
	PNG_SIG(fp);
	header_chunk(s->width, s->height, color_type, fp);
	if (color_type == PNG_RGB) {
		data_chunks_rgb(s->width, s->height, s->data, fp);
	}
	else if (color_type == PNG_RGBA) {
		data_chunks_rgb(s->width, s->height, s->data, fp);
	}
	else {
		fprintf(stderr, "Error: unknown color type\n");
		exit(EXIT_FAILURE);
	}
	end_chunk(fp);
	fclose(fp);
}

// breaks on width = height = 38000; need to fix