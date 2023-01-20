#pragma once

#include <ncurses.h>

#include <mutex>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "timer.h"

class WindowBase {
public:
    WindowBase(int x, int y, int w, int h);
    void SetGeometry(int x, int y, int w, int h);
    void SetColor(Color color);
    virtual void Draw();

    WINDOW* GetWindow() { return win_; }

protected:
    WINDOW* border_;
    WINDOW* win_;
    int width_;
    int height_;
    int x_;
    int y_;
    Color color_;
};

class NextWindow : public WindowBase {
public:
    NextWindow();

    void Draw() final;
};

struct TextBox {
    std::string text;
    int x;
    int y;
};

class DashBoard : public WindowBase {
public:
    DashBoard();
    void Draw() final;

    void AddStore(int store);
    void UpdateApm();
    void UpdateBps();

private:
    int store_;
    double bps_;
    double apm_;
};

// TODO(jowu); Move to slots.cc or snapshot.cc
struct SnapShot {
    void Serialize(const Dot pool[][WINDOW_WIDTH]);
    void Deserialize(Dot pool[][WINDOW_WIDTH]);

    std::unordered_map<int, Color> filled_colors;
    Type type;
    int64_t timestamp_ms;
};

class SnapShotManager {
public:
    SnapShotManager() = default;

    void TakeSnapshot(const Dot pool[][WINDOW_WIDTH]);
    bool ResumeFromSnapShot(Dot pool[][WINDOW_WIDTH]);

private:
private:
    std::stack<SnapShot> history;
};

class PoolWindow : public WindowBase {
public:
    PoolWindow();

    void Draw() final;
    void Start();

    bool enable_key;

private:
    bool OnTick();
    void OnKey(int key_code, int pos_x_shift);

private:
    std::mutex mu_;
    std::unique_ptr<Timer> timer_;
    std::unique_ptr<SnapShotManager> snapshots_;
    std::thread input_thread_;
    double interval_ms_;
    int pos_x_shift_;
    // TODO(jowu): bitset is faster
    Dot slots_[WINDOW_HEIGHT][WINDOW_WIDTH];
};

class MainWindow {
public:
    MainWindow();

    void Refresh();
    void Play();

private:
    bool InitCurses();
    bool InitKey();

private:
    double fps_;

    std::unique_ptr<NextWindow> next_window_;
    std::unique_ptr<DashBoard> dashboard_;
    std::unique_ptr<PoolWindow> pool_window_;
};
