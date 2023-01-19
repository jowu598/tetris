#include "common.h"

#include <ncurses.h>

#include <map>

#include "log.h"

void ShowSlotPos(Dot pool[][WINDOW_WIDTH]) {
    char msg[1024];
    int offset = snprintf(msg, 1024, "%s", "--------------------------\n");
    for (int h = 0; h < WINDOW_HEIGHT; ++h) {
        for (int w = 0; w < WINDOW_WIDTH; ++w) {
            offset += snprintf(msg + offset, 1024 - offset, "%d",
                               pool[h][w].is_filled);
        }

        offset += snprintf(msg + offset, 1024 - offset, "\n");
    }
    LOG(msg);
}

void ShowBlockPos(const BlockPoints& ps) {
    int a[16] = {0};
    for (int i = 0; i < 4; ++i) {
        int x = ps.x[i] + 1;
        int y = ps.y[i] + 1;
        a[x * 4 + y] = 1;
    }

    char msg[1024];
    int offset = snprintf(msg, 1024, "%s", "--------------------------\n");
    for (int i = 0; i < 16; ++i) {
        offset += snprintf(msg + offset, 1024 - offset, "%d ", a[i]);
        if ((i + 1) % 4 == 0) {
            offset += snprintf(msg + offset, 1024 - offset, "%s", "\n");
        }
    }

    LOG(msg);
}

bool IsMovable(const Block& block, Dot pool[][WINDOW_WIDTH]) {
    return IsMovable(block, block.GetX(), block.GetY(), pool);
}

bool IsRotatable(const Block& block, bool clockwise, Dot pool[][WINDOW_WIDTH]) {
    auto cur_block = block;
    cur_block.Rotate(clockwise);
    return IsMovable(cur_block, pool);
}

bool IsMovable(const Block& block, int x, int y, Dot pool[][WINDOW_WIDTH]) {
    for (int i = 0; i < 4; ++i) {
        int pos_x = x + block.GetPoints().x[i] + 1;
        int pos_y = y + block.GetPoints().y[i] + 1;
        if (pos_x < 0 || pos_x >= WINDOW_WIDTH) {
            return false;
        }

        if (pos_y < 0 || pos_y >= WINDOW_HEIGHT) {
            return false;
        }

        if (pool[pos_y][pos_x].is_filled) {
            return false;
        }
    }

    return true;
}

bool DropBlock(const Block& block, Dot pool[][WINDOW_WIDTH]) {
    int h = block.GetY();
    for (; h <= WINDOW_HEIGHT; ++h) {
        if (!IsMovable(block, block.GetX(), h, pool)) {
            break;
        }
    }

    if (h == 0) {
        // Not movable at beginning.
        return false;
    }

    for (int i = 0; i < 4; ++i) {
        int x = block.GetX() + block.GetPoints().x[i] + 1;
        int y = h + block.GetPoints().y[i];
        pool[y][x].is_filled = true;
        pool[y][x].color = block.GetColor();
    }

    return true;
}

bool IsCompleted(Dot pool[][WINDOW_WIDTH], std::vector<int>* lines) {
    if (!pool || !lines) {
        return false;
    }

    for (int h = WINDOW_HEIGHT - 1; h >= 0; --h) {
        bool clean = true;
        for (int w = 0; w < WINDOW_WIDTH; ++w) {
            if (!pool[h][w].is_filled) {
                clean = false;
                break;
            }
        }

        if (clean) {
            lines->push_back(h);
            LOG("cccccccccccc %d", h);
        }
    }

    return lines->size();
}

struct MapKeyCompare {
    bool operator()(const int lhs, const int rhs) const { return lhs > rhs; }
};

void ClearLines(Dot pool[][WINDOW_WIDTH]) {
    if (!pool) {
        return;
    }

    ShowSlotPos(pool);

    std::vector<int> lines;
    if (IsCompleted(pool, &lines)) {
        // Collapse clean lines.
        /* old   shift      new
         * 0 c   5
         * 1 1
         * 2 1
         * 3 b   3
         * 4 1
         * 5 1              5 c
         * 6 a   1          6 b
         * 7 1              7 a
         */

        // Key: current height from here, Value: target height shift to.
        std::map<int, int, MapKeyCompare> shift_lines;
        int shift = 0;
        for (int h = WINDOW_HEIGHT - 1; h >= 0; --h) {
            if (lines[shift] == h) {
                ++shift;
            } else {
                shift_lines[h] = h + shift;
                LOG("0000000000000000000 %d ------ %d", h, shift);
            }
        }

        for (auto x : shift_lines) {
            LOG("xxxxxxxx %d >> %d", x.first, x.second);

            for (int w = 0; w < WINDOW_WIDTH; ++w) {
                pool[x.second][w] = pool[x.first][w];
            }
        }

        // Empty forehead.
        for (int i = 0; i < lines.size(); ++i) {
            for (int j = 0; j < WINDOW_WIDTH; ++j) {
                // FIXME: empty color.
                pool[i][j].color = 0;
                pool[i][j].is_filled = false;
            }
        }
    }

    ShowSlotPos(pool);
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
        next_points->y[i] = cur_points.x[i];
    }

    // ShowBlockPos(*next_points);
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
