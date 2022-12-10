#include "common.h"

#include <ncurses.h>

bool IsMovable(const Block& block, Dot pool[][WINDOW_WIDTH]) {
    bool res = false;

    for (int i = 0; i < 4; ++i) {
        int x = block.GetX() + block.GetPoints().x[i] + 1;
        int y = block.GetY() + block.GetPoints().y[i] + 1;
        if (x < 0 || x >= WINDOW_WIDTH) {
            return false;
        }

        if (y < 0 || y >= WINDOW_HEIGHT) {
            return false;
        }

        if (pool[y][x].is_filled) {
            return false;
        }
    }

    return true;
}

bool DropBlock(const Block& block, Dot pool[][WINDOW_WIDTH]) {
    Block temp = block;
    int h = WINDOW_HEIGHT - 1;
    for (int i = 0; i < block.GetY(); ++i) {
        temp.SetY(h - i);
        if (IsMovable(temp, pool)) {
            for (int i = 0; i < 4; ++i) {
                int x = temp.GetX() + temp.GetPoints().x[i] + 1;
                int y = temp.GetY() + temp.GetPoints().y[i] + 1;
                pool[y][x].is_filled = true;
                pool[y][x].color = block.GetColor();
            }

            return true;
        }
    }

    return false;
}

bool IsCompleted(Dot pool[][WINDOW_WIDTH], std::vector<int>* lines) {
    if (!pool || !lines) {
        return false;
    }

    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        bool clean = false;
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            if (!pool[y][x].is_filled) {
                break;
            }
        }

        if (clean) {
            lines->push_back(y);
        }
    }

    return lines->size();
}

void ClearLines(Dot pool[][WINDOW_WIDTH]) {
    if (!pool) {
        return;
    }

    std::vector<int> lines;
    if (IsCompleted(pool, &lines)) {
        // Collapse clean lines.
        int shift = 0;
        for (int h = 0; h < WINDOW_HEIGHT; ++h) {
            for (int w = 0; w < WINDOW_WIDTH; ++w) {
                pool[WINDOW_HEIGHT - 1 - h][w] =
                    pool[WINDOW_HEIGHT - 1 - h - shift][w];
            }

            if (lines[shift] == WINDOW_HEIGHT - 1 - h) {
                ++shift;
            }
        }

        // Empty forehead.
        for (int i = 0; i < lines.size(); ++i) {
            for (int j = 0; j < WINDOW_HEIGHT; ++j) {
                // FIXME: empty color.
                pool[i][j].color = 0;
                pool[i][j].is_filled = false;
            }
        }
    }
}

void RotateBlock(const BlockPoints& cur_points, BlockPoints* next_points) {
    /* y(sin(a))
     * ^    /
     * |   /
     * |  /
     * | /
     * |/a
     * --------------> x(cos(a))
     *
     *
     */
    // sin(a+90=cos(a) ------>  y' = x
    // cos(a+90)=-sin(a) ---->  x' = -y
    // Anchor point (1,1) set to (0,0)
    for (int i = 0; i < 4; ++i) {
        next_points->x[i] = -cur_points.y[i];
        next_points->x[i] = cur_points.x[i];
    }
}

int GetWidth() {
    int h, w;
    getmaxyx(stdscr, h, w);
    return w;
}

int GetHeight() {
    int h, w;
    getmaxyx(stdscr, h, w);
    return h;
}
