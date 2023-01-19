#pragma once

#include "block.h"

bool IsMovable(const Block& block, Dot pool[][WINDOW_WIDTH]);
bool IsMovable(const Block& block, int x, int y, Dot pool[][WINDOW_WIDTH]);

bool IsRotatable(const Block& block, bool clockwise, Dot pool[][WINDOW_WIDTH]);

bool DropBlock(const Block& block, Dot pool[][WINDOW_WIDTH]);
bool IsCompleted(Dot pool[][WINDOW_WIDTH], std::vector<int>* lines);
void ClearLines(Dot pool[][WINDOW_WIDTH]);

void RotateBlock(const BlockPoints& cur_points, BlockPoints* next_points);

int GetWidth();
int GetHeight();

#define MARK_SINGLETON(cls)            \
public:                                \
    inline static cls* GetInstance() { \
        static cls s_instance;         \
        return &s_instance;            \
    };
