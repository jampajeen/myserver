/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <random>
#include <atomic>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>

#if (__cplusplus > 199711L)
#define CPP11
#endif

#define CPP_BEGIN_DECLARE       extern "C" {
#define CPP_END_DECLARE         }

#define HAVE_SELECT             0x00000001

#if defined(__linux__) && defined(UnderTestingWithSelect)
#define HAVE_EPOLL              0x00000004
#endif

#if defined(__sun) && defined(__SVR4)
#define HAVE_PORT               0x00000008
#endif

#if defined(__WIN32) || defined(__WIN64)
#define HAVE_IOCP               0x00000100
#endif

#if (defined(__unix__) && !defined(__linux__)) || (defined(__APPLE__) && defined(__MACH__))
#define HAVE_KQUEUE             0x00010000
#endif

#define MAX_BUF_SIZE    (1024 * 64)
#define MAX_CONNECTION  (1024 * 64)

#ifdef CPP11
#define THREAD_ID std::this_thread::get_id()
#else
#define THREAD_ID gettid()
#endif

#define PROCESS_ID getpid()

#ifndef NULL
#define NULL 0
#endif

typedef unsigned char uchar_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;
typedef unsigned long ulong_t;
typedef unsigned short ushort_t;
/* bits types */
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;

typedef enum {
    SUCCESS = 0,
    ERROR = 1,
    ERROR_QUEUE_FULL
} ret_t;

inline uint16_t _htons(uint16_t n) {
    return ((n & 0xff00) >> 8) | ((n & 0x00ff) << 8);
}

inline uint16_t _ntohs(uint16_t n) {
    return ((n & 0xff00) >> 8) | ((n & 0x00ff) << 8);
}

inline uint32_t _htonl(uint32_t n) {
    return ((n & 0xff000000) >> 24) | ((n & 0x00ff0000) >> 8) | ((n & 0x0000ff00) << 8) | ((n & 0x000000ff) << 24);
}

inline uint32_t _ntohl(uint32_t n) {
    return ((n & 0xff000000) >> 24) | ((n & 0x00ff0000) >> 8) | ((n & 0x0000ff00) << 8) | ((n & 0x000000ff) << 24);
}

inline uint64_t _htonll(uint64_t n) {
    return (uint64_t) _htonl(n >> 32) | ((uint64_t) _htonl(n) << 32);
}

inline uint64_t _ntohll(uint64_t n) {
    return (uint64_t) _htonl(n >> 32) | ((uint64_t) _htonl(n) << 32);
}

#define logger(level, format, ...) fprintf(stdout, "%s|%s|%d|%d|%s|%d|%s|" format "\n", level, __TIME__, PROCESS_ID, THREAD_ID, __FILE__, __LINE__,__func__, ##__VA_ARGS__)
#define LOG(format, ...) fprintf(stdout, format "\n", ##__VA_ARGS__)
#ifdef debug
#define DEBUG(format, ...) logger("DEBUG", format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...) {}
#endif
#define INFO(format, ...) logger("INFO", format, ##__VA_ARGS__)
#define WARN(format, ...) logger("WARN", format, ##__VA_ARGS__)
#define ERROR(format, ...) logger("ERROR", format, ##__VA_ARGS__)
#define FATAL(format, ...) logger("FATAL", format, ##__VA_ARGS__)

typedef void(*task_callback_func)(void* arg);

#endif /* COMMON_H */
