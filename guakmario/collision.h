#pragma once
#include "data.h"

bool isSolidTile(int tile);
bool IsColliding(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh);
void CheckFlagCollision();
void CheckClearCollision();
void CheckCoinCollision();
