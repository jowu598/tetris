#include "key.h"

#include <assert.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"

namespace {
// TODO(jowu): Support auto check for x input vendor.
constexpr char kInputPath[] = "/dev/input/event3";
}  // namespace

KeyHandler* KeyHandler::s_instance_ = nullptr;

KeyHandler* KeyHandler::GetInstance() {
    if (!s_instance_) {
        s_instance_ = new KeyHandler();
    }

    return s_instance_;
}

bool KeyHandler::Init() {
    LOG_HERE;
    quit_ = false;
    auto rfd = open(kInputPath, O_RDONLY);
    assert(rfd != -1);
    LOG_HERE;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(rfd, &fds);
    th_ = std::thread([&]() {
        while (!quit_) {
            select(rfd + 1, &fds, NULL, NULL, NULL);
            struct input_event evt;
            auto size = sizeof(struct input_event);
            ssize_t rd = read(rfd, &evt, size);
            LOG("rd %d ", rd);
            if (rd == size) {
                LOG("key code %d type %d value %d", evt.code, evt.type,
                    evt.value);
                for (const auto& cb : cbs_) {
                    cb(evt.code);
                }
            }
        }
    });

    return true;
}

void KeyHandler::DeInit() {
    quit_ = true;
    if (th_.joinable()) {
        th_.join();
    }
}

void KeyHandler::RegisterNotifier(KeyEventNotifier cb) { cbs_.push_back(cb); }

