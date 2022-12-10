#pragma once

#include "block.h"

bool IsMovable(const Block& block, Dot pool[][WINDOW_WIDTH]);
bool DropBlock(const Block& block, Dot pool[][WINDOW_WIDTH]);
bool IsCompleted(Dot pool[][WINDOW_WIDTH], std::vector<int>* lines);
void ClearLines(Dot pool[][WINDOW_WIDTH]);

void RotateBlock(const BlockPoints& cur_points, BlockPoints* next_points);

int GetWidth();
int GetHeight();
