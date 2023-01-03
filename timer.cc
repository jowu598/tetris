#include "timer.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

#include "log.h"

Timer::Timer(TimeOutFunc func) : interval_ms_(0), func_(func), quit_(false) {
    tfd_ = timerfd_create(CLOCK_REALTIME, 0);
    assert(tfd_ != -1);
}

void Timer::SetInterval(double interval_ms) {
    LOG("inter is %f", interval_ms);
    interval_ms_ = interval_ms;
    int sec = static_cast<int>(interval_ms_);
    int nano = (interval_ms_ - sec) * 1e9;
    struct timespec now;
    struct itimerspec v;
    assert(clock_gettime(CLOCK_REALTIME, &now) != -1);
    v.it_value.tv_sec = now.tv_sec + sec;
    v.it_value.tv_nsec = now.tv_nsec;
    v.it_interval.tv_sec = sec;
    v.it_interval.tv_nsec = nano;
    assert(timerfd_settime(tfd_, TFD_TIMER_ABSTIME, &v, NULL) != -1);
}

void Timer::Start(double interval_ms) {
    SetInterval(interval_ms);
    ssize_t rd;
    uint64_t exp;
    th_ = std::thread([&]() {
        while (true) {
            rd = read(tfd_, &exp, sizeof(uint64_t));
            assert(rd != -1);
            func_();
        }
    });
}

void Timer::Stop() {
    quit_ = true;
    if (th_.joinable()) {
        th_.join();
    }
}
