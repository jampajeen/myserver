/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */

#ifndef CRYPTO_H
#define CRYPTO_H

#include "common.h"

using namespace std;

void xorstring(const char *key, char *string, int stringlen);
string md5(const char *src, const size_t srclen);
string sha1(const char *src, const size_t srclen);

#endif /* CRYPTO_H */
