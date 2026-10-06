#pragma once
#include "data.h"

bool isSolidTile(int tile);
bool IsColliding(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh);
bool IsColliding_item(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh);
void checkcollision_flag();
void checkcollision_clear();
void checkcollision_coin();
