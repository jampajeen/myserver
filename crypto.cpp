#include <string>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "crypto.h"

void xorstring(const char *key, char *string, int stringlen) {
    int keylen = strlen(key);
    for (int i = 0; i < stringlen; i++) {
        string[i] = string[i] ^ key[i % keylen];
    }
}

string md5(const char *src, const size_t srclen) {
    uchar_t result[MD5_DIGEST_LENGTH];
    MD5((const uchar_t*) src, srclen, result);

    char buffer[33];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&buffer[i * 2], "%02x", (uint_t) result[i]);
    }

    buffer[32] = 0;
    return string(buffer);
}

string sha1(const char *src, const size_t srclen) {
    uchar_t result[SHA_DIGEST_LENGTH];
    SHA1((const uchar_t*) src, srclen, result);

    char buffer[41];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&buffer[i * 2], "%02x", (uint_t) result[i]);
    }

    buffer[40] = 0;
    return string(buffer);
}
