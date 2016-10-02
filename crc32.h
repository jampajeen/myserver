
#ifndef CRC32_H
#define CRC32_H
#include "common.h"

uint32_t crc32(const void *buf, size_t bufsize, uint32_t crc);
bool checksum(const uint8_t *buf, size_t bufsize, uint32_t crc);

#endif /* CRC32_H */
