#include "timer.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

Timer::Timer(double interval_ms, TimeOutFunc func)
    : interval_ms_(interval_ms), func_(func), quit_(false) {
    tfd_ = timerfd_create(CLOCK_REALTIME, 0);
}

void Timer::SetInterval(double interval_ms) {
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

void Timer::Start() {
    SetInterval(interval_ms_);
    th_ = std::thread([&]() {
        uint64_t exp;
        while (!quit_) {
            assert(read(tfd_, &exp, sizeof(uint64_t)) != -1);
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

// int main() {
//    Timer t(1.5, []() { printf("timeout %lf\n", Nano()); });
//    t.Start();
//    return 0;
//}
