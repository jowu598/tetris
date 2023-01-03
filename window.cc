#include "window.h"

#include <ncurses.h>
#include <unistd.h>

#include <functional>
#include <utility>

#include "common.h"
#include "key.h"
#include "log.h"

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

NextWindow::NextWindow() : WindowBase(GetWidth() - 14, 0, 14, 20) {}

void NextWindow::Draw() {
    // Draw blocks.
    int offset = 0;
    for (auto block : BlockCreator::GetInstance()->GetNextBlocks()) {
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

PoolWindow::PoolWindow()
    : WindowBase(GetWidth() / 2 - WINDOW_WIDTH / 2, 0, WINDOW_WIDTH,
                 WINDOW_HEIGHT),
      interval_ms_(0.1),
      timer_(std::make_unique<Timer>(std::bind(&PoolWindow::OnTick, this))) {}

void PoolWindow::Start() { timer_->Start(interval_ms_); }

void PoolWindow::Draw() {
    //   std::unique_lock<std::mutex> lock(mu_);
    WindowBase::Draw();
    // Draw pool.
    {
        for (int y = 0; y < WINDOW_HEIGHT; ++y) {
            for (int x = 0; x < WINDOW_WIDTH; ++x) {
                wattrset(win_, COLOR_PAIR(0));
                // LOG("draw at %d %d", y + ps.y[i] + 1, 2 * (x + ps.x[i] + 1));
                mvwaddch(win_, y, 2 * x, ' ');
                mvwaddch(win_, y, 2 * x + 1, ' ');
            }
        }
        // Draw block.
        {
            int offset = 0;
            auto cur_block = BlockCreator::GetInstance()->GetCurrentBlock();
            int y = cur_block->GetY();
            int x = cur_block->GetX();
            auto ps = cur_block->GetPoints();
            wattrset(win_, COLOR_PAIR(cur_block->GetColor()));

            for (int i = 0; i < 4; ++i) {
                // LOG("draw at %d %d", y + ps.y[i] + 1, 2 * (x + ps.x[i] + 1));
                mvwaddch(win_, y + ps.y[i] + 1, 2 * (x + ps.x[i] + 1), ' ');
                mvwaddch(win_, y + ps.y[i] + 1, 2 * (x + ps.x[i] + 1) + 1, ' ');
            }
        }

        wrefresh(win_);
        // Draw slots.
    }
}

void PoolWindow::OnTick() {
    //    std::unique_lock<std::mutex> lock(mu_);
    LOG("tick");
    auto cur_block = BlockCreator::GetInstance()->GetCurrentBlock();
    if (cur_block) {
        // Let block down.
        if (IsMovable(*cur_block.get(), slots_)) {
            cur_block->Move(Action::DOWN);
        } else {
            LOG("cannot move");
        }
    }
}

MainWindow::MainWindow() : fps_(60.0) {
    InitCurses();
    // InitKey();
    next_window_ = std::make_unique<NextWindow>();
    // dashboard_ = std::make_unique<DashBoard>();
    pool_window_ = std::make_unique<PoolWindow>();
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

bool MainWindow::InitKey() {
    KeyHandler::GetInstance()->Init();
    KeyHandler::GetInstance()->RegisterNotifier(
        std::bind(&MainWindow::OnKeyEvent, this, std::placeholders::_1));
    return true;
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
    pool_window_->Start();
    while (1) {
        Refresh();
        sleep(1 / fps_);
    }
}
