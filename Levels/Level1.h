#pragma once
#include "../Game.h"
#include <vector>

// Увеличиваем размеры, чтобы сделать "зум" поближе
struct Player {
    float x, y;
    float vx, vy;
    int size = 50; 
    bool onGround = false;
};

struct Platform {
    float x, y, w, h;
};

struct Spike {
    float x, y;
    int w, h;
    bool pointingDown; // true = шипы на потолке, false = на полу
};

// Инициализация и логика
void InitLevel1();
void UpdateLevel1();
void RenderLevel1(HDC hdc);
