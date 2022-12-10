#pragma once

#include <ncurses.h>

#include <list>
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
    std::shared_ptr<Block> PopFront();

private:
    static constexpr int kNextBlockNumber = 4;
    std::list<std::shared_ptr<Block>> next_blocks_;
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
    friend class MainWindow;

public:
    PoolWindow();

    void UpdateBlock(std::shared_ptr<Block> block) { cur_block_ = block; }
    void Draw() final;
    //    bool Create();
    //    void Destroy();
private:
    //    bool InitCurses();
private:
    // TODO(jowu): bitset is faster.
    Dot slots_[WINDOW_HEIGHT][WINDOW_WIDTH];
    std::shared_ptr<Block> cur_block_;
};

class MainWindow {
public:
    MainWindow();
    void OnTick();
    void OnKeyEvent(int key_code);

    void Refresh();
    void Show();

private:
    bool InitCurses();
    bool InitTimer();
    bool InitKey();

private:
    double fps_;
    bool interval_ms_;
    std::unique_ptr<Timer> timer_;

    std::unique_ptr<NextWindow> next_window_;
    std::unique_ptr<DashBoard> dashboard_;
    std::unique_ptr<PoolWindow> pool_window_;
};
