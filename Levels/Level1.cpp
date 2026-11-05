#include "Level1.h"
#include <cmath>

using namespace Gdiplus;

const float LEVEL_WIDTH = 2500.0f;
const float LEVEL_HEIGHT = 1000.0f;
float cameraX = 0;
float cameraY = 0;

std::vector<Platform> platforms;
std::vector<Spike> spikes;
Player player;

// Зона финиша (те самые стрелочки <<)
float finishZoneX = 50, finishZoneY = 150, finishZoneW = 100, finishZoneH = 100;

// Для смерти в пропасти
bool isFallingToDeath = false;
float deathTimer = 0;

void RespawnPlayer() {
    player.x = 100;
    player.y = 850; // Спавн внизу слева
    player.vx = 0;
    player.vy = 0;
    cameraX = 0;
    cameraY = 400; // Камера сразу смотрит на игрока
    isFallingToDeath = false;
    deathTimer = 0;
}

void InitLevel1() {
    platforms.clear();
    spikes.clear();

    // --- ВОССОЗДАНИЕ КАРТЫ ПО СКЕТЧУ ---
    
    // 1. Стартовая площадка (слева внизу)
    platforms.push_back({0, 900, 400, 100});

    // 2. Шипы над стартом (на потолке левой пещеры)
    for (int i = 0; i < 4; i++) spikes.push_back({200.0f + i * 40, 250, 40, 20, true});

    // 3. Первая пропасть
    // (Здесь просто пустота, игрок должен прыгать по платформам)

    // 4. Парящие блоки посередине
    platforms.push_back({500, 750, 100, 30});
    platforms.push_back({650, 600, 100, 30});
    platforms.push_back({800, 750, 100, 30}); // Тут шипы сверху? На рисунке квадратики. Пусть будут просто блоки.

    // 5. Правая часть (Длинный пол с ямами и конструкциями)
    platforms.push_back({1000, 900, 400, 100}); // Пол
    // Шипы на полу
    for (int i = 0; i < 3; i++) spikes.push_back({1100.0f + i * 40, 880, 40, 20, false});

    platforms.push_back({1500, 900, 1000, 100}); // Длинный пол до конца

    // 6. Конструкция с "крышей" и шипами (по центру справа)
    platforms.push_back({1300, 600, 300, 30}); // Крыша
    for (int i = 0; i < 4; i++) spikes.push_back({1350.0f + i * 40, 630, 40, 20, true}); // Шипы под крышей
    platforms.push_back({1350, 750, 200, 30}); // Платформа под крышей

    // 7. Ступеньки вверх (справа)
    platforms.push_back({1900, 800, 100, 100});
    platforms.push_back({2000, 700, 100, 200});
    platforms.push_back({2100, 600, 100, 300});
    platforms.push_back({2200, 500, 100, 400}); // Верхняя площадка

    // 8. Длинный потолок (по которому нужно идти налево)
    platforms.push_back({500, 200, 1700, 50});
    
    // Шипы на потолке (свисают вниз)
    for (int i = 0; i < 5; i++) spikes.push_back({1000.0f + i * 40, 250, 40, 20, true});
    for (int i = 0; i < 4; i++) spikes.push_back({1500.0f + i * 40, 250, 40, 20, true});

    // Парящие платформы на пути к финишу (верхний левый угол)
    platforms.push_back({800, 350, 100, 30});
    platforms.push_back({600, 450, 100, 30});
    platforms.push_back({400, 350, 100, 30});
    platforms.push_back({200, 250, 100, 30}); // Прямо под финишем

    // Потолок слева
    platforms.push_back({0, 50, 500, 50});

    RespawnPlayer();
}

void UpdateLevel1() {
    // Если уже умираем в пропасти, ждем немного и показываем Game Over
    if (isFallingToDeath) {
        deathTimer += 1.0f;
        if (deathTimer > 60.0f) { // Примерно 1 секунда
            g_state = STATE_GAME_OVER;
        }
        return; // Отключаем управление
    }

    // Управление
    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) player.vx = -6;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) player.vx = 6;
    else player.vx = 0;

    if ((GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) && player.onGround) {
        player.vy = -17;
        player.onGround = false;
    }

    player.vy += 1.2f; 
    player.x += player.vx;

    // Коллизия по X
    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vx > 0) player.x = p.x - player.size;
            if (player.vx < 0) player.x = p.x + p.w;
        }
    }

    if (player.x < 0) player.x = 0;
    if (player.x + player.size > LEVEL_WIDTH) player.x = LEVEL_WIDTH - player.size;

    player.y += player.vy;
    player.onGround = false;

    // Коллизия по Y
    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vy > 0) { 
                player.y = p.y - player.size; 
                player.vy = 0; 
                player.onGround = true; 
            }
            else if (player.vy < 0) { 
                player.y = p.y + p.h; 
                player.vy = 0; 
            }
        }
    }

    // Проверка финиша (две стрелочки <<)
    if (player.x < finishZoneX + finishZoneW && player.x + player.size > finishZoneX &&
        player.y < finishZoneY + finishZoneH && player.y + player.size > finishZoneY) {
        g_state = STATE_COMPLETE;
    }

    // Проверка падения в пропасть (если упал ниже экрана)
    if (player.y > LEVEL_HEIGHT) {
        isFallingToDeath = true; // Включаем режим падения
    }

    // Проверка шипов
    for (const auto& s : spikes) {
        if (player.x < s.x + s.w && player.x + player.size > s.x &&
            player.y < s.y + s.h && player.y + player.size > s.y) {
            // Если наткнулся на шипы, просто респавним (или тоже Game Over, как хочешь)
            RespawnPlayer(); 
            break;
        }
    }

    // --- ЛОГИКА КАМЕРЫ ---
    if (!isFallingToDeath) {
        // Камера следует за игроком
        float targetCamX = player.x + player.size / 2 - 400; 
        float targetCamY = player.y + player.size / 2 - 300; 

        cameraX += (targetCamX - cameraX) * 0.1f;
        cameraY += (targetCamY - cameraY) * 0.1f;
    } else {
        // Если падаем в пропасть, камера по Y ОСТАНАВЛИВАЕТСЯ (как ты и просил)
        float targetCamX = player.x + player.size / 2 - 400; 
        cameraX += (targetCamX - cameraX) * 0.1f; // По X еще немного следит
        // По Y камера замирает на последнем значении
    }

    // Ограничители камеры
    if (cameraX < 0) cameraX = 0;
    if (cameraX > LEVEL_WIDTH - 800) cameraX = LEVEL_WIDTH - 800;
    if (cameraY < 0) cameraY = 0;
    if (cameraY > LEVEL_HEIGHT - 600) cameraY = LEVEL_HEIGHT - 600;
}

void RenderLevel1(HDC hdc) {
    Graphics graphics(hdc);
    
    // БЕЛЫЙ ФОН (как ты просил)
    SolidBrush bg(Color(255, 255, 255, 255));
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    // 1. Платформы (Черные)
    SolidBrush platBrush(Color(255, 0, 0, 0));
    for (const auto& p : platforms) {
        if (p.x + p.w > cameraX && p.x < cameraX + 800 && p.y + p.h > cameraY && p.y < cameraY + 600) {
            graphics.FillRectangle(&platBrush, (REAL)(p.x - cameraX), (REAL)(p.y - cameraY), (REAL)p.w, (REAL)p.h);
        }
    }

    // 2. Шипы (Красные)
    SolidBrush spikeBrush(Color(255, 200, 50, 50)); 
    for (const auto& s : spikes) {
        if (s.x + s.w > cameraX && s.x < cameraX + 800 && s.y + s.h > cameraY && s.y < cameraY + 600) {
            PointF points[3];
            float drawX = s.x - cameraX;
            float drawY = s.y - cameraY;

            if (s.pointingDown) { 
                points[0] = PointF(drawX, drawY);
                points[1] = PointF(drawX + s.w, drawY);
                points[2] = PointF(drawX + s.w / 2, drawY + s.h);
            } else { 
                points[0] = PointF(drawX, drawY + s.h);
                points[1] = PointF(drawX + s.w, drawY + s.h);
                points[2] = PointF(drawX + s.w / 2, drawY);
            }
            graphics.FillPolygon(&spikeBrush, points, 3);
        }
    }

    // 3. Зона финиша (Стрелочки <<)
    SolidBrush finishBrush(Color(255, 0, 0, 0));
    // Рисуем просто две галочки (как на скетче) или текст
    FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font font(&fontFamily, 40, FontStyleBold);
    graphics.DrawString(L"<<", -1, &font, PointF(finishZoneX - cameraX, finishZoneY - cameraY), &finishBrush);

    // 4. Игрок (Черный куб)
    SolidBrush playerBrush(Color(255, 0, 0, 0));
    graphics.FillRectangle(&playerBrush, (REAL)(player.x - cameraX), (REAL)(player.y - cameraY), (REAL)player.size, (REAL)player.size);
    
    // Белая обводка (чтобы видеть на черных платформах)
    Pen whitePen(Color(255, 255, 255, 255), 2);
    graphics.DrawRectangle(&whitePen, (INT)(player.x - cameraX), (INT)(player.y - cameraY), (INT)player.size, (INT)player.size);
}

// Экран GAME OVER
void RenderGameOver(HDC hdc) {
    Graphics graphics(hdc);
    SolidBrush bg(Color(255, 255, 255, 255)); // Белый фон
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    SolidBrush textBrush(Color(255, 0, 0, 0)); // Черный текст
    FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font font(&fontFamily, 48, FontStyleBold);
    
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    
    graphics.DrawString(L"GAME OVER\n\n(Нажми мышкой, чтобы выйти в меню)", -1, &font, 
        RectF(0, 0, 800, 600), &format, &textBrush);
}

// Экран COMPLETE
void RenderComplete(HDC hdc) {
    Graphics graphics(hdc);
    SolidBrush bg(Color(255, 255, 255, 255)); // Белый фон
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    SolidBrush textBrush(Color(255, 0, 0, 0)); // Черный текст
    FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font font(&fontFamily, 48, FontStyleBold);
    
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    
    graphics.DrawString(L"COMPLETE\n\n(Нажми мышкой, чтобы выйти в меню)", -1, &font, 
        RectF(0, 0, 800, 600), &format, &textBrush);
}
