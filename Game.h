#pragma once
#include <windows.h>
#include <gdiplus.h>

// Состояния игры
enum GameState {
    STATE_DISCLAIMER,
    STATE_MENU,
    STATE_PLAYING
};

// Глобальные переменные (доступны во всех файлах)
extern GameState g_state;
extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern bool g_running;

// Функции Меню (Menu/Menu.cpp)
void InitMenu();
void UpdateMenu();
void RenderMenu(HDC hdc);

// Функции Уровня 1 (Levels/Level1.cpp)
void InitLevel1();
void UpdateLevel1();
void RenderLevel1(HDC hdc);
