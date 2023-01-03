#pragma once
#include <stdint.h>

#include <functional>
#include <thread>

typedef std::function<void()> TimeOutFunc;

int64_t Nano();

class Timer {
public:
    explicit Timer(TimeOutFunc func);
    void Start(double interval_ms);
    void Stop();
    void SetInterval(double interval_ms);

private:
    double interval_ms_;
    TimeOutFunc func_;
    int tfd_;
    bool quit_;
    std::thread th_;
};
