#pragma once

#include <ncurses.h>

#include <list>
#include <mutex>
#include <string>
#include <vector>

#include "common.h"
#include "timer.h"

class WindowBase {
public:
    WindowBase(int x, int y, int w, int h);
    void SetGeometry(int x, int y, int w, int h);
    void SetColor(Color color);
    virtual void Draw();

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

class PoolWindow : public WindowBase {
public:
    PoolWindow();

    void Draw() final;
    void Start();

private:
    void OnTick();

private:
    std::mutex mu_;
    std::unique_ptr<Timer> timer_;
    double interval_ms_;
    // TODO(jowu): bitset is faster.
    Dot slots_[WINDOW_HEIGHT][WINDOW_WIDTH];
};

class MainWindow {
public:
    MainWindow();
    void OnKeyEvent(int key_code);

    void Refresh();
    void Show();

private:
    bool InitCurses();
    bool InitKey();

private:
    double fps_;

    std::unique_ptr<NextWindow> next_window_;
    std::unique_ptr<DashBoard> dashboard_;
    std::unique_ptr<PoolWindow> pool_window_;
};
