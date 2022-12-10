#include <functional>
#include <thread>
#include <vector>

typedef std::function<void(int code)> KeyEventNotifier;

typedef void (*KeyEventNotifier2)();
class KeyHandler {
public:
    static KeyHandler* GetInstance();

    void RegisterNotifier(KeyEventNotifier cb);

    bool Init();
    void DeInit();

private:
    KeyHandler() = default;

private:
    static KeyHandler* s_instance_;
    std::vector<KeyEventNotifier> cbs_;
    std::thread th_;
    int fd_;
    bool quit_;
};
