#include "window.h"

#include <curses.h>
#include <ncurses.h>
#include <unistd.h>

#include <functional>
#include <utility>

#include "common.h"
#include "log.h"

WindowBase::WindowBase(int x, int y, int w, int h)
    : x_(x), y_(y), width_(w), height_(h) {
    LOG("border %d %d", width_, height_);
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

NextWindow::NextWindow() : WindowBase(GetWidth() - 12, 0, 12, 20) {}

void NextWindow::Draw() {
    // Draw pool.
    {
        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                wattrset(win_, COLOR_PAIR(0));

                mvwaddch(win_, y, 2 * x, ' ');
                mvwaddch(win_, y, 2 * x + 1, ' ');
            }
        }
    }
    // Draw blocks.
    int offset = 0;
    for (auto const& block : BlockCreator::GetInstance()->GetNextBlocks()) {
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
    : WindowBase(GetWidth() / 2 - WINDOW_WIDTH, 0, 2 * WINDOW_WIDTH,
                 WINDOW_HEIGHT),
      interval_ms_(1),
      timer_(std::make_unique<Timer>(std::bind(&PoolWindow::OnTick, this))) {
    input_thread_ = std::thread([&]() {
        while (1) {
            if (enable_key) {
                int ch = wgetch(win_);
                OnKey(ch);
            } else {
                LOG("key disabled!!");
            }
        }
    });
}

void PoolWindow::Start() { timer_->Start(interval_ms_); }

void PoolWindow::Draw() {
    WindowBase::Draw();
    // Draw pool.
    {
        for (int y = 0; y < WINDOW_HEIGHT; ++y) {
            for (int x = 0; x < WINDOW_WIDTH; ++x) {
                if (!slots_[y][x].is_filled) {
                    wattrset(win_, COLOR_PAIR(0));
                } else {
                    wattrset(win_, COLOR_PAIR(slots_[y][x].color));
                }

                mvwaddch(win_, y, 2 * x, ' ');
                mvwaddch(win_, y, 2 * x + 1, ' ');
            }
        }
    }
    // Draw block.
    {
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
}

bool PoolWindow::OnTick() {
    std::unique_lock<std::mutex> lock;
    auto cur_block = BlockCreator::GetInstance()->GetCurrentBlock();
    if (cur_block) {
        // Check if over.
        // if (!IsMovable(*cur_block.get(), slots_)) {
        //    LOG("end of game");
        //    // exit(0);
        //    return true;
        //}
        // Let block down.
        if (IsMovable(*cur_block.get(), cur_block->GetX(),
                      cur_block->GetY() + 1, slots_)) {
            cur_block->Move(Action::DOWN);
        } else {
            // Check if over.
            auto ps = cur_block->GetPoints();
            auto x = cur_block->GetX();
            auto y = cur_block->GetY();
            LOG("cannot move block %d %d", x, y);
            for (int i = 0; i < 4; ++i) {
                LOG("[%d %d] filled", y, x);
                slots_[y + ps.y[i] + 1][x + ps.x[i] + 1].is_filled = true;
                slots_[y + ps.y[i] + 1][x + ps.x[i] + 1].color =
                    cur_block->GetColor();
            }

            BlockCreator::GetInstance()->CreateNext();
        }
    }

    return false;
}

void PoolWindow::OnKey(int key_code) {
    std::unique_lock<std::mutex> lock;
    auto cur_block = BlockCreator::GetInstance()->GetCurrentBlock();
    // TODO(jowu): DashBoard for key settings.
    switch (key_code) {
        case 32:  // SPACE->drop.
            if (DropBlock(*cur_block.get(), slots_)) {
                ClearLines(slots_);
                BlockCreator::GetInstance()->CreateNext();
            }
            break;
        case 104:  // H->left.
            if (IsMovable(*cur_block.get(), cur_block->GetX() - 1,
                          cur_block->GetY(), slots_)) {
                cur_block->Move(Action::LEFT);
            }
            break;
        case 108:  // L->right.
            if (IsMovable(*cur_block.get(), cur_block->GetX() + 1,
                          cur_block->GetY(), slots_)) {
                cur_block->Move(Action::RIGHT);
            }
            break;
        case 110:  // N->down.
            cur_block->Move(Action::DOWN);
            break;
        case 106:  // J->rotate clockwise.
            cur_block->Rotate(true);
            break;
        case 107:  // K->rotate anti-clockwise.
            cur_block->Rotate(false);
            break;
            //        case 107:  // B->flick left.
            //            FlickLeft(*cur_block.get(), slots_);
            //            break;
            //        case 107:  // B->flick right.
            //            FlickRight(*cur_block.get(), slots_);
            //            break;
        default:
            break;
    }
}

MainWindow::MainWindow() : fps_(10.0) {
    InitCurses();
    next_window_ = std::make_unique<NextWindow>();
    // dashboard_ = std::make_unique<DashBoard>();
    pool_window_ = std::make_unique<PoolWindow>();
}

bool MainWindow::InitCurses() {
    initscr();
    curs_set(0);
    atexit([]() { endwin(); });
    keypad(stdscr, FALSE);
    nodelay(stdscr, TRUE);
    nonl();
    noecho();
    cbreak();

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_CYAN);
    init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(6, COLOR_BLACK, COLOR_BLUE);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);
    init_pair(17, COLOR_RED, COLOR_BLACK);
    init_pair(18, COLOR_YELLOW, COLOR_BLACK);
    init_pair(19, COLOR_GREEN, COLOR_BLACK);
    init_pair(20, COLOR_YELLOW, COLOR_BLACK);
    init_pair(21, COLOR_WHITE, COLOR_BLACK);
    init_pair(22, COLOR_WHITE, COLOR_BLACK);

    return true;
}

void MainWindow::Refresh() {
    next_window_->Draw();
    // dashboard_->Draw();
    pool_window_->Draw();
}

void MainWindow::Play() {
    pool_window_->Start();
    while (1) {
        // TODO(jowu):Ignore key input when refresh.
        // int ch = wgetch(pool_window_->GetWindow());
        //        LOG("ch %d", ch);
        // Skip key event when rendering.
        Refresh();
        pool_window_->enable_key = true;
        sleep(1 / fps_);
        pool_window_->enable_key = false;
    }
}
