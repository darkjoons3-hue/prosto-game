#include "Level1.h"
#include <cmath>

using namespace Gdiplus;

// --- Константы уровня ---
const float LEVEL_WIDTH = 2500.0f;  // Длина уровня
const float LEVEL_HEIGHT = 800.0f;  // Высота уровня
float cameraX = 0;
float cameraY = 0;

// --- Объекты ---
std::vector<Platform> platforms;
std::vector<Spike> spikes;
Player player;
float markX = 2200, markY = 700; // Марк в конце уровня

// Функция для респавна
void RespawnPlayer() {
    player.x = 100;
    player.y = 600; // Появление на земле
    player.vx = 0;
    player.vy = 0;
}

void InitLevel1() {
    platforms.clear();
    spikes.clear();

    // 1. ПОЛ (с пропастями)
    platforms.push_back({0, 750, 800, 50});      // Земля слева
    platforms.push_back({1000, 750, 1500, 50});  // Земля справа

    // 2. ПОТОЛОК
    platforms.push_back({0, 0, LEVEL_WIDTH, 50});

    // 3. ПЛАТФОРМЫ (Парящие в воздухе)
    platforms.push_back({400, 600, 200, 30});
    platforms.push_back({700, 500, 150, 30});
    platforms.push_back({950, 400, 200, 30});
    platforms.push_back({1250, 300, 150, 30});
    platforms.push_back({1500, 450, 300, 30});
    platforms.push_back({1900, 550, 200, 30});

    // 4. СТУПЕНЬКИ (Справа, как на рисунке)
    platforms.push_back({2100, 650, 100, 100});
    platforms.push_back({2200, 550, 100, 200});
    platforms.push_back({2300, 450, 100, 300});

    // 5. ШИПЫ (На полу и на потолке)
    // Шипы на полу
    for (int i = 0; i < 5; i++) spikes.push_back({800.0f + i * 40, 730, 40, 20, false});
    for (int i = 0; i < 4; i++) spikes.push_back({1300.0f + i * 40, 730, 40, 20, false});
    
    // Шипы на потолке
    for (int i = 0; i < 6; i++) spikes.push_back({600.0f + i * 40, 50, 40, 20, true});
    for (int i = 0; i < 5; i++) spikes.push_back({1700.0f + i * 40, 50, 40, 20, true});
    // Шипы на платформах
    for (int i = 0; i < 3; i++) spikes.push_back({1520.0f + i * 40, 430, 40, 20, true});

    RespawnPlayer();
}

void UpdateLevel1() {
    // --- 1. Управление ---
    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) player.vx = -6;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) player.vx = 6;
    else player.vx = 0;

    if ((GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) && player.onGround) {
        player.vy = -17;
        player.onGround = false;
    }

    // --- 2. Физика и Коллизии ---
    player.vy += 1.2f; // Гравитация
    player.x += player.vx;

    // Коллизия по X
    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vx > 0) player.x = p.x - player.size;
            if (player.vx < 0) player.x = p.x + p.w;
        }
    }

    // Ограничение по краям уровня
    if (player.x < 0) player.x = 0;
    if (player.x + player.size > LEVEL_WIDTH) player.x = LEVEL_WIDTH - player.size;

    player.y += player.vy;
    player.onGround = false;

    // Коллизия по Y
    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vy > 0) { // Падение вниз
                player.y = p.y - player.size; 
                player.vy = 0; 
                player.onGround = true; 
            }
            else if (player.vy < 0) { // Удар головой
                player.y = p.y + p.h; 
                player.vy = 0; 
            }
        }
    }

    // Если упал в пропасть
    if (player.y > LEVEL_HEIGHT) RespawnPlayer();

    // --- 3. Проверка шипов (Смерть) ---
    for (const auto& s : spikes) {
        if (player.x < s.x + s.w && player.x + player.size > s.x &&
            player.y < s.y + s.h && player.y + player.size > s.y) {
            RespawnPlayer(); // Возвращаем на старт
            break;
        }
    }

    // --- 4. Логика Камеры (Слежение) ---
    // Камера центрируется на игроке
    float targetCamX = player.x + player.size / 2 - 400; // 400 = половина ширины экрана
    float targetCamY = player.y + player.size / 2 - 300; // 300 = половина высоты экрана

    // Плавное движение камеры (интерполяция)
    cameraX += (targetCamX - cameraX) * 0.1f;
    cameraY += (targetCamY - cameraY) * 0.1f;

    // Ограничиваем камеру, чтобы она не показывала пустоту за краями уровня
    if (cameraX < 0) cameraX = 0;
    if (cameraX > LEVEL_WIDTH - 800) cameraX = LEVEL_WIDTH - 800;
    if (cameraY < 0) cameraY = 0;
    if (cameraY > LEVEL_HEIGHT - 600) cameraY = LEVEL_HEIGHT - 600;
}

void RenderLevel1(HDC hdc) {
    Graphics graphics(hdc);
    
    // Фон (темно-серый)
    SolidBrush bg(Color(255, 30, 30, 35));
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    // --- ВАЖНО: ВСЁ РИСУЕМ СО СМЕЩЕНИЕМ НА КАМЕРУ (X - cameraX, Y - cameraY) ---

    // 1. Платформы
    SolidBrush platBrush(Color(255, 100, 100, 100));
    for (const auto& p : platforms) {
        // Оптимизация: рисуем только то, что видно на экране
        if (p.x + p.w > cameraX && p.x < cameraX + 800 &&
            p.y + p.h > cameraY && p.y < cameraY + 600) {
            graphics.FillRectangle(&platBrush, p.x - cameraX, p.y - cameraY, p.w, p.h);
        }
    }

    // 2. Шипы (Рисуем треугольники)
    SolidBrush spikeBrush(Color(255, 200, 50, 50)); // Красные шипы
    for (const auto& s : spikes) {
        if (s.x + s.w > cameraX && s.x < cameraX + 800 && s.y + s.h > cameraY && s.y < cameraY + 600) {
            PointF points[3];
            float drawX = s.x - cameraX;
            float drawY = s.y - cameraY;

            if (s.pointingDown) { // Шипы с потолка
                points[0] = PointF(drawX, drawY);
                points[1] = PointF(drawX + s.w, drawY);
                points[2] = PointF(drawX + s.w / 2, drawY + s.h);
            } else { // Шипы на полу
                points[0] = PointF(drawX, drawY + s.h);
                points[1] = PointF(drawX + s.w, drawY + s.h);
                points[2] = PointF(drawX + s.w / 2, drawY);
            }
            graphics.FillPolygon(&spikeBrush, points, 3);
        }
    }

    // 3. Марк (Зеленый куб)
    SolidBrush markBrush(Color(255, 0, 255, 0));
    graphics.FillRectangle(&markBrush, markX - cameraX, markY - cameraY, 50, 50);

    // 4. Игрок (Черный куб)
    SolidBrush playerBrush(Color(255, 0, 0, 0));
    graphics.FillRectangle(&playerBrush, player.x - cameraX, player.y - cameraY, player.size, player.size);
    
    // Обводка игрока
    Pen whitePen(Color(255, 255, 255, 255), 2);
    graphics.DrawRectangle(&whitePen, player.x - cameraX, player.y - cameraY, player.size, player.size);
}
