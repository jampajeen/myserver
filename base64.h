/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */
#ifndef BASE64_H
#define	BASE64_H

#include "common.h"

size_t base64_decode (char *decoded, size_t decodedlen, const char *src, size_t srclen);
size_t base64_encode(char* encoded, size_t encodedlen, const char* src, size_t srclen);

#endif	/* BASE64_H */

