#include "Menu.h"
#include <cmath>

// --- Локальные переменные меню ---
float g_disclaimerAlpha = 0.0f;
float g_menuAlpha = 0.0f;
POINT g_mousePos = { 400, 300 };
POINT g_lastMousePos = { -1, -1 };
float g_cursorAngle = 0.0f;
float g_glitchTimer = 0.0f;
RECT g_glitchRect = { 0, 0, 0, 0 };

using namespace Gdiplus;

void DrawTextAlpha(HDC hdc, const wchar_t* text, RECT rc, float alpha, int fontSize, bool bold, Color color) {
    Graphics graphics(hdc);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font font(&fontFamily, fontSize, bold ? FontStyleBold : FontStyleRegular, UnitPixel);
    SolidBrush brush(Color((BYTE)(alpha * 255), color.GetR(), color.GetG(), color.GetB()));
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    graphics.DrawString(text, -1, &font, RectF((float)rc.left, (float)rc.top, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top)), &format, &brush);
}

void DrawCustomCursor(HDC hdc) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    float size = 15.0f;
    PointF p1(g_mousePos.x + cos(g_cursorAngle) * size, g_mousePos.y + sin(g_cursorAngle) * size);
    PointF p2(g_mousePos.x + cos(g_cursorAngle + 2.5f) * size, g_mousePos.y + sin(g_cursorAngle + 2.5f) * size);
    PointF p3(g_mousePos.x + cos(g_cursorAngle - 2.5f) * size, g_mousePos.y + sin(g_cursorAngle - 2.5f) * size);
    PointF points[3] = { p1, p2, p3 };
    SolidBrush fillBrush(Color(255, 0, 0, 0));
    graphics.FillPolygon(&fillBrush, points, 3);
    Pen outlinePen(Color(255, 255, 255, 255), 1.5f); // Белая обводка для черного фона
    graphics.DrawPolygon(&outlinePen, points, 3);
}

void InitMenu() {
    g_disclaimerAlpha = 0.0f;
    g_menuAlpha = 0.0f;
}

void UpdateMenu() {
    // 1. Обновление курсора
    POINT currentMouse;
    GetCursorPos(&currentMouse);
    ScreenToClient(g_hWnd, &currentMouse);

    if (currentMouse.x != g_lastMousePos.x || currentMouse.y != g_lastMousePos.y) {
        g_cursorAngle = atan2((float)(currentMouse.y - g_lastMousePos.y), (float)(currentMouse.x - g_lastMousePos.x));
        g_lastMousePos = currentMouse;
        g_mousePos = currentMouse;
    }

    // 2. Анимации
    if (g_state == STATE_DISCLAIMER) {
        if (g_disclaimerAlpha < 1.0f) g_disclaimerAlpha += 0.01f;
    } else if (g_state == STATE_MENU) {
        if (g_menuAlpha < 1.0f) g_menuAlpha += 0.02f;
        
        g_glitchTimer += 1.0f;
        if (g_glitchTimer > 15.0f) {
            g_glitchTimer = 0;
            int boxX = (800 - 150) / 2;
            int boxY = 150;
            int gx = boxX + 10 + rand() % 130;
            int gy = boxY + 10 + rand() % 130;
            g_glitchRect = { gx, gy, gx + 8, gy + 8 };
        }
    }
}

void RenderMenu(HDC hdc) {
    Graphics graphics(hdc);

    if (g_state == STATE_DISCLAIMER) {
        SolidBrush bg(Color(255, 15, 15, 15));
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        RECT textRect = { 50, 100, 750, 450 };
        const wchar_t* disclaimer = 
            L"ВНИМАНИЕ\n\n"
            L"Данный программный продукт не является вирусом\nи не нанесет вреда вашей системе.\n\n"
            L"Однако игра содержит элементы психологического ужаса,\nрезкие звуки, мерцания экрана и взаимодействие\nс вашим рабочим столом.\n\n"
            L"Если у вас слабая нервная система или проблемы с сердцем,\nнемедленно закройте приложение.\n\n"
            L"Нажимая «Я СОГЛАСЕН», вы подтверждаете,\nчто готовы к полному погружению.";

        DrawTextAlpha(hdc, disclaimer, textRect, g_disclaimerAlpha, 20, false, Color(255, 200, 200, 200));

        if (g_disclaimerAlpha > 0.5f) {
            RECT btnRect = { 300, 500, 500, 550 };
            bool hover = (g_mousePos.x >= btnRect.left && g_mousePos.x <= btnRect.right && g_mousePos.y >= btnRect.top && g_mousePos.y <= btnRect.bottom);
            SolidBrush btnBrush(hover ? Color(255, 50, 50, 50) : Color(255, 30, 30, 30));
            graphics.FillRectangle(&btnBrush, btnRect.left, btnRect.top, 200, 50);
            DrawTextAlpha(hdc, L"Я СОГЛАСЕН", btnRect, 1.0f, 18, true, Color(255, 255, 255, 255));
        }
    } 
    else if (g_state == STATE_MENU) {
        SolidBrush bg(Color(255, 255, 255, 255)); // Белый фон
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        // Логотип
        SolidBrush logoBrush(Color(255, 0, 0, 0));
        graphics.FillRectangle(&logoBrush, 325, 150, 150, 150);

        // Глитч
        if (g_glitchRect.right > 0) {
            SolidBrush glitchBrush(Color(255, 0, 255, 0));
            graphics.FillRectangle(&glitchBrush, g_glitchRect.left, g_glitchRect.top, 8, 8);
        }

        // Кнопки
        const wchar_t* buttons[] = { L"PLAY", L"ONITION", L"EXIT" };
        for (int i = 0; i < 3; i++) {
            RECT btnRect = { 300, 350 + (i * 50), 500, 390 + (i * 50) };
            bool hover = (g_mousePos.x >= btnRect.left && g_mousePos.x <= btnRect.right && g_mousePos.y >= btnRect.top && g_mousePos.y <= btnRect.bottom);
            Color textColor = hover ? Color(255, 0, 180, 0) : Color(255, 0, 0, 0);
            DrawTextAlpha(hdc, buttons[i], btnRect, g_menuAlpha, 24, true, textColor);
        }
    }

    DrawCustomCursor(hdc);
}
