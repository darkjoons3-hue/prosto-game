#pragma execution_character_set("utf-8")
#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN

#include "Game.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/ENTRY:wWinMainCRTStartup")

using namespace Gdiplus;

void OnMouseClick(int x, int y) {
    if (g_state == STATE_DISCLAIMER) {
        RECT btnRect = { 300, 500, 500, 550 };
        if (x >= btnRect.left && x <= btnRect.right && y >= btnRect.top && y <= btnRect.bottom) {
            g_state = STATE_MENU;
        }
    } else if (g_state == STATE_MENU) {
        for (int i = 0; i < 3; i++) {
            RECT btnRect = { 300, 350 + (i * 50), 500, 390 + (i * 50) };
            if (x >= btnRect.left && x <= btnRect.right && y >= btnRect.top && y <= btnRect.bottom) {
                if (i == 0) { 
                    g_state = STATE_PLAYING;
                    InitLevel1();
                } else if (i == 2) { 
                    g_running = false;
                }
            }
        }
    } else if (g_state == STATE_GAME_OVER || g_state == STATE_COMPLETE) {
        g_state = STATE_MENU;
        InitMenu();
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        OnMouseClick(pt.x, pt.y);
        return 0;
    }
    case WM_SETCURSOR:
        SetCursor(nullptr);
        return TRUE;
    case WM_DESTROY:
        PostQuitMessage(0);
        g_running = false;
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int) {
    g_hInst = hInst;

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    ShowCursor(FALSE);

    WNDCLASSW wc = {};
    wc.hInstance = hInst;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = L"SquareGameWnd";
    RegisterClassW(&wc);

    // --- ВОЗВРАЩАЕМ РЕЖИМ БЕЗ РАМОК (ИДЕАЛЬНО ДЛЯ WINLATOR) ---
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    g_hWnd = CreateWindowExW(WS_EX_TOPMOST, L"SquareGameWnd", L"Square Black and Green", 
        WS_POPUP | WS_VISIBLE, 
        0, 0, screenW, screenH, // Растягиваем на весь экран Winlator
        nullptr, nullptr, hInst, nullptr);

    // Принудительно выводим окно на передний план
    SetForegroundWindow(g_hWnd);
    SetFocus(g_hWnd);

    InitMenu();

    HDC hdc = GetDC(g_hWnd);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, 800, 600);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

    MSG msg;
    while (g_running) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) g_running = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (g_state == STATE_DISCLAIMER || g_state == STATE_MENU) UpdateMenu();
        else if (g_state == STATE_PLAYING) UpdateLevel1();

        if (g_state == STATE_DISCLAIMER || g_state == STATE_MENU) RenderMenu(memDC);
        else if (g_state == STATE_PLAYING) RenderLevel1(memDC);
        else if (g_state == STATE_GAME_OVER) RenderGameOver(memDC);
        else if (g_state == STATE_COMPLETE) RenderComplete(memDC);

        // Растягиваем отрисовку на экран (или центрируем)
        BitBlt(hdc, 0, 0, 800, 600, memDC, 0, 0, SRCCOPY);
        Sleep(16); 
    }

    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
    ReleaseDC(g_hWnd, hdc);
    GdiplusShutdown(gdiplusToken);
    return 0;
}
