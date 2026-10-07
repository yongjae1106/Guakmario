#pragma once
#include <windows.h>

void HandleGameKeyDown(WPARAM wParam);
void HandleGameKeyUp(WPARAM wParam);
void HandleGameChar(WPARAM wParam);
void TickCooldowns();
