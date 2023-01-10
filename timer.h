#pragma once
#include <stdint.h>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

typedef std::function<bool()> TimeOutFunc;

int64_t Nano();

class Timer {
public:
    explicit Timer(TimeOutFunc func);
    void Start(double interval_ms);
    void Stop();
    void SetInterval(double interval_ms);

private:
    std::mutex mu_;
    std::condition_variable cv_;
    double interval_ms_;
    TimeOutFunc func_;
    int tfd_;
    bool quit_;
    std::thread th_;
};
