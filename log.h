#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

int64_t GetNano();
void WriteToFile(const char* file, const char* func, int64_t line,
                 const char* fmt, ...);

#define LOG(fmt, ...)                                                      \
    do {                                                                   \
        WriteToFile(__FILENAME__, __func__, __LINE__, fmt, ##__VA_ARGS__); \
    } while (0)
