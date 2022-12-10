#include "window.h"

#include <ncurses.h>
#include <unistd.h>

#include <functional>
#include <utility>

#include "common.h"
#include "key.h"
#include "log.h"

int s_screen_w = 200;
int s_screen_h = 40;

WindowBase::WindowBase(int x, int y, int w, int h)
    : x_(x), y_(y), width_(w), height_(h) {
    border_ = newwin(height_ + 2, width_ + 2, y_, x_);
    win_ = derwin(border_, height_, width_, 1, 1);

    Draw();
    color_ = 7;
}

void WindowBase::SetGeometry(int x, int y, int w, int h) {}

void WindowBase::SetColor(Color color) {}

void WindowBase::Draw() {
    box(border_, 0, 0);
    wrefresh(border_);
}

NextWindow::NextWindow() : WindowBase(GetWidth() - 14, 0, 14, 20) {
    BlockCreator creator;
    for (int i = 0; i < kNextBlockNumber; ++i) {
        next_blocks_.push_back(creator.CreateRandom());
    }
}

void NextWindow::Draw() {
    // Draw blocks.
    int offset = 0;
    for (auto block : next_blocks_) {
        int y = 1 + offset;
        int x = 2;
        offset += 5;
        auto ps = block->GetPoints();
        wattrset(win_, COLOR_PAIR(block->GetColor()));

        for (int i = 0; i < 4; ++i) {
            mvwaddch(win_, y + ps.y[i] + 1, 2 * (x + ps.x[i] + 1), ' ');
            mvwaddch(win_, y + ps.y[i] + 1, 2 * (x + ps.x[i] + 1) + 1, ' ');
        }
    }

    WindowBase::Draw();
}

std::shared_ptr<Block> NextWindow::PopFront() {
    auto block = next_blocks_.front();
    next_blocks_.pop_front();
    BlockCreator creator;
    next_blocks_.push_back(creator.CreateRandom());
    return block;
}

// PoolWindow::PoolWindow() : WindowBase(0, 0, GetWidth() - 14, GetHeight()) {}
PoolWindow::PoolWindow() : WindowBase(GetWidth() / 2 - 12, 0, 24, 20) {}

void PoolWindow::Draw() {
    // Draw block.
    int offset = 0;
    int y = cur_block_->GetY();
    int x = cur_block_->GetX();
    auto ps = cur_block_->GetPoints();
    wattrset(win_, COLOR_PAIR(cur_block_->GetColor()));

    for (int i = 0; i < 4; ++i) {
        mvwaddch(win_, y + ps.y[i] + 1, 2 * (x + ps.x[i] + 1), ' ');
        mvwaddch(win_, y + ps.y[i] + 1, 2 * (x + ps.x[i] + 1) + 1, ' ');
    }

    WindowBase::Draw();

    // Draw slots.
}

MainWindow::MainWindow()
    : fps_(60.0),
      interval_ms_(1.0),
      timer_(std::make_unique<Timer>(interval_ms_,
                                     std::bind(&MainWindow::OnTick, this))) {
    InitCurses();
    InitTimer();
    // InitKey();
    next_window_ = std::make_unique<NextWindow>();
    // dashboard_ = std::make_unique<DashBoard>();
    pool_window_ = std::make_unique<PoolWindow>();

    auto block = next_window_->PopFront();
    pool_window_->UpdateBlock(block);
}

bool MainWindow::InitCurses() {
    initscr();
    curs_set(0);
    atexit([]() { endwin(); });
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    nonl();
    noecho();
    cbreak();

    start_color();
    /* 1 - 16 is for blocks */
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_CYAN);
    init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(6, COLOR_BLACK, COLOR_BLUE);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);    /* 17 - ? is for other things */
    init_pair(17, COLOR_RED, COLOR_BLACK);     // points
    init_pair(18, COLOR_YELLOW, COLOR_BLACK);  // number of lines
    init_pair(19, COLOR_GREEN, COLOR_BLACK);   // level
    init_pair(20, COLOR_YELLOW, COLOR_BLACK);  // messages
    init_pair(21, COLOR_WHITE, COLOR_BLACK);   // window borders
    init_pair(22, COLOR_WHITE, COLOR_BLACK);   // end of line animation

    return true;
}

bool MainWindow::InitTimer() {
    timer_->Start();
    return true;
}

bool MainWindow::InitKey() {
    KeyHandler::GetInstance()->Init();
    KeyHandler::GetInstance()->RegisterNotifier(
        std::bind(&MainWindow::OnKeyEvent, this, std::placeholders::_1));
    return true;
}

void MainWindow::OnTick() {
    // Let block down.
    //    if (IsMovable(*cur_block_.get(), pool_window_->slots_)) {
    //        cur_block_->Move(Action::DOWN);
    //    }
}

void MainWindow::OnKeyEvent(int key_code) {}

void MainWindow::Refresh() {
    // std::unique_ptr<NextWindow> next_window_;
    // std::unique_ptr<DashBoard> dashboard_;
    // std::unique_ptr<PoolWindow> pool_window_;
    //
    next_window_->Draw();
    // dashboard_->Draw();
    pool_window_->Draw();
}

void MainWindow::Show() {
    int screen_h, screen_w;
    getmaxyx(stdscr, s_screen_h, s_screen_w);
    LOG("screen size is %d %d", s_screen_w, s_screen_h);

    // Main thread for UI.
    while (1) {
        Refresh();
        sleep(1 / fps_);
    }
}
