#include "log.h"

#include <sys/time.h>

#include <fstream>

namespace {
constexpr int kMaxMessageLength = 4096;
int64_t VsPrintfLogMessage(const char* file, const char* func, int line,
                           const char* fmt, va_list* args, std::string* buf) {
    int64_t ts = GetNano();

    char log[kMaxMessageLength] = {0};
    int64_t offset = 0;

    offset += snprintf(log + offset, kMaxMessageLength - offset, "[%s:%d,%s]",
                       file, line, func);
    va_list args_clone;
    va_copy(args_clone, *args);
    offset +=
        vsnprintf(log + offset, kMaxMessageLength - offset, fmt, args_clone);
    va_end(args_clone);
    if (offset < kMaxMessageLength && (log[offset - 1] != '\n')) {
        log[offset] = '\n';
    }

    if (offset > kMaxMessageLength) {
        offset = kMaxMessageLength - 1;
    }

    buf->assign(log, offset + 1);
    return offset;
}
}  // namespace

int64_t GetNano() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec * 1e9 + now.tv_usec * 1e3);
}

void WriteToFile(const char* file, const char* func, int64_t line,
                 const char* fmt, ...) {
    std::string buf;
    int64_t ts = GetNano();
    std::string log;

    va_list args;
    va_start(args, fmt);

    VsPrintfLogMessage(file, func, line, fmt, &args, &log);

    va_end(args);

    std::ofstream ofs("log", std::ios_base::out | std::ios_base::app);
    ofs << std::string(log) << std::endl;
    ofs.flush();
    ofs.close();
}

