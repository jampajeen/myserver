
#include "base64.h"
/*
 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 * http://opensource.apple.com//source/QuickTimeStreamingServer/QuickTimeStreamingServer-452/CommonUtilitiesLib/base64.c
 */

static const unsigned char ltable[] = {
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 64, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 62, 66, 66, 66, 63, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, 66, 66, 66, 65, 66, 66, 66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 66, 66, 66, 66, 66, 66, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66
};

static const char WHITESPACE = 64;
static const char EQUAL = 65;
static const char INVALID = 66;
static const char BASE64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char PAD64 = '=';

inline static int cal_encoded_buf_len(size_t srclen) {
    return ((srclen + 2) / 3 * 4) + 1;
}

inline static int cal_decoded_buf_len(size_t srclen) {
    return (srclen / 4 * 3) + 1;
}

size_t base64_decode(char *decoded, size_t decodedlen, const char *src, size_t srclen) {
    const char *end = src + srclen;
    char iter = 0;
    size_t buf = 0, len = 0;

    while (src < end) {
        uchar_t c = ltable[*src++];
        switch (c) {
            case WHITESPACE: continue; /* skip whitespace */
            case INVALID:
                return -1; /* invalid input, return error */
            case EQUAL: /* pad character, end of data */
                src = end;
                continue;
            default:
                buf = buf << 6 | c;
                iter++; // increment the number of iteration
                /* If the buffer is full, split it into bytes */
                if (iter == 4) {
                    if ((len += 3) > decodedlen) {
                        return -1; /* buffer overflow */
                    }
                    *(decoded++) = (buf >> 16) & 255;
                    *(decoded++) = (buf >> 8) & 255;
                    *(decoded++) = buf & 255;
                    buf = 0;
                    iter = 0;

                }
        }
    }

    if (iter == 3) {
        if ((len += 2) > decodedlen) {
            return -1; /* buffer overflow */
        }
        *(decoded++) = (buf >> 10) & 255;
        *(decoded++) = (buf >> 2) & 255;

    } else if (iter == 2) {
        if (++len > decodedlen) {
            return -1; /* buffer overflow */
        }
        *(decoded++) = (buf >> 4) & 255;

    }

    *decoded = 0;
    return len;
}

size_t base64_encode(char* encoded, size_t encodedlen, const char* src, size_t srclen) {
    const uint8_t *data = (const uint8_t *) src;
    size_t len = 0;
    size_t x;
    uint32_t n = 0;
    int padCount = srclen % 3;
    uint8_t n0, n1, n2, n3;

    if (encodedlen < cal_encoded_buf_len(srclen)) return -1;
    
    /* increment over the length of the string, three characters at a time */
    for (x = 0; x < srclen; x += 3) {
        /* these three 8-bit (ASCII) characters become one 24-bit number */
        n = ((uint32_t) data[x]) << 16; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0

        if ((x + 1) < srclen)
            n += ((uint32_t) data[x + 1]) << 8; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0

        if ((x + 2) < srclen)
            n += data[x + 2];

        /* this 24-bit number gets separated into four 6-bit numbers */
        n0 = (uint8_t) (n >> 18) & 63;
        n1 = (uint8_t) (n >> 12) & 63;
        n2 = (uint8_t) (n >> 6) & 63;
        n3 = (uint8_t) n & 63;

        /*
         * if we have one byte available, then its encoding is spread
         * out over two characters
         */
        if (len >= encodedlen) return -1; /* indicate failure: buffer too small */
        encoded[len++] = BASE64[n0];
        if (len >= encodedlen) return -1; /* indicate failure: buffer too small */
        encoded[len++] = BASE64[n1];

        /*
         * if we have only two bytes available, then their encoding is
         * spread out over three chars
         */
        if ((x + 1) < srclen) {
            if (len >= encodedlen) return -1; /* indicate failure: buffer too small */
            encoded[len++] = BASE64[n2];
        }

        /*
         * if we have all three bytes available, then their encoding is spread
         * out over four characters
         */
        if ((x + 2) < srclen) {
            if (len >= encodedlen) return -1; /* indicate failure: buffer too small */
            encoded[len++] = BASE64[n3];
        }
    }

    /*
     * create and add padding that is required if we did not have a multiple of 3
     * number of characters available
     */
    if (padCount > 0) {
        for (; padCount < 3; padCount++) {
            if (len >= encodedlen) return -1; /* indicate failure: buffer too small */
            encoded[len++] = '=';
        }
    }

    if (len >= encodedlen) return -1; /* indicate failure: buffer too small */
    encoded[len] = 0;
    return len; /* indicate success */
}
