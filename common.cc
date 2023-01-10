#include "common.h"

#include <ncurses.h>

#include "log.h"

bool IsMovable(const Block& block, Dot pool[][WINDOW_WIDTH]) {
    return IsMovable(block, block.GetX(), block.GetY(), pool);
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

    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        bool clean = true;
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            if (!pool[y][x].is_filled) {
                clean = false;
                break;
            }
        }

        if (clean) {
            lines->push_back(y);
            LOG("cccccccccccc %d", y);
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
            if (lines[shift] == WINDOW_HEIGHT - 1 - h) {
                ++shift;
            }

            for (int w = 0; w < WINDOW_WIDTH; ++w) {
                pool[WINDOW_HEIGHT - 1 - h][w] =
                    pool[WINDOW_HEIGHT - 1 - h - shift][w];
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
