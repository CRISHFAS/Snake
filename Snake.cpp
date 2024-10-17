#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

int nScreenWidth = 120;
int nScreenHeight = 30;

struct sSnakeSegment
{
    int x;
    int y;
};

class Snake_PGE : public olc::PixelGameEngine {
public:
    Snake_PGE() {
        sAppName = "Snake_PGE";
    }

private:
    std::list<sSnakeSegment> snake = { {60,15},{61,15},{62,15},{63,15},{64,15},{65,15},{66,15},{67,15},{68,15},{69,15} };
    int nFoodX = 30;
    int nFoodY = 15;
    int nScore = 0;
    int nSnakeDirection = 3; // 0 = UP, 1 = RIGHT, 2 = DOWN, 3 = LEFT
    bool bDead = false;

    float fElapsedTimeCumulated = 0.0f;

protected:

    void ResetGame() {
        snake = { {60,15},{61,15},{62,15},{63,15},{64,15},{65,15},{66,15},{67,15},{68,15},{69,15} };
        nFoodX = 30;
        nFoodY = 15;
        nScore = 0;
        nSnakeDirection = 3;
        bDead = false;
        fElapsedTimeCumulated = 0.0f;
    }

    virtual bool OnUserCreate() {
        srand(time(0));
        ResetGame();
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) {

        #define TICK_X 0.120f
        #define TICK_Y 0.200f

        float fLocalTick = ((nSnakeDirection % 2 == 1) ? TICK_X : TICK_Y);
        fElapsedTimeCumulated += fElapsedTime;

        if (!bDead && fElapsedTimeCumulated > fLocalTick) {
            fElapsedTimeCumulated -= fLocalTick;

            // Movimiento de la serpiente solo con flechas
            if (GetKey(olc::Key::UP).bHeld && nSnakeDirection != 2) {
                nSnakeDirection = 0; // Arriba
            }
            if (GetKey(olc::Key::DOWN).bHeld && nSnakeDirection != 0) {
                nSnakeDirection = 2; // Abajo
            }
            if (GetKey(olc::Key::RIGHT).bHeld && nSnakeDirection != 3) {
                nSnakeDirection = 1; // Derecha
            }
            if (GetKey(olc::Key::LEFT).bHeld && nSnakeDirection != 1) {
                nSnakeDirection = 3; // Izquierda
            }

            // Mover la serpiente
            switch (nSnakeDirection) {
                case 0: snake.push_front({ snake.front().x    , snake.front().y - 1 }); break; // UP
                case 1: snake.push_front({ snake.front().x + 1, snake.front().y     }); break; // RIGHT
                case 2: snake.push_front({ snake.front().x    , snake.front().y + 1 }); break; // DOWN
                case 3: snake.push_front({ snake.front().x - 1, snake.front().y     }); break; // LEFT
            }

            // Comida
            if (snake.front().x == nFoodX && snake.front().y == nFoodY) {
                nScore++;
                nFoodX = rand() % (ScreenWidth() / 8);
                nFoodY = (rand() % ((ScreenHeight() / 8) - 4)) + 3;
                for (int i = 0; i < 5; i++)
                    snake.push_back({ snake.back().x, snake.back().y });
            }

            // Detectar colisiones
            if (snake.front().x < 0 || snake.front().x >= nScreenWidth) bDead = true;
            if (snake.front().y < 3 || snake.front().y >= nScreenHeight) bDead = true;

            for (auto i = snake.begin(); i != snake.end(); i++)
                if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
                    bDead = true;

            snake.pop_back(); // Quitar la cola
        }

        // Dibujar en la pantalla
        Clear(olc::BLACK);

        auto CGE_Draw = [this](int x, int y, char c, olc::Pixel col = olc::WHITE) {
            DrawString(x * 8, y * 8, std::string(1, c), col);
        };
        auto CGE_DrawString = [this](int x, int y, std::string s, olc::Pixel col = olc::WHITE) {
            DrawString(x * 8, y * 8, s, col);
        };

        // Dibujar borde superior e inferior
        for (int i = 1; i < ScreenWidth() - 1; i++) {
            CGE_Draw(i, 0, '=', olc::DARK_YELLOW);
            CGE_Draw(i, 2, '=', olc::DARK_YELLOW);
        }

        // Puntaje
        CGE_DrawString(5, 1, "S N A K E ! ! SCORE: " + std::to_string(nScore), olc::YELLOW);

        // Dibujar la serpiente
        for (auto s : snake)
            CGE_Draw(s.x, s.y, bDead ? '+' : '0', olc::DARK_CYAN);

        CGE_Draw(snake.front().x, snake.front().y, bDead ? 'X' : '@', olc::CYAN);
        CGE_Draw(nFoodX, nFoodY, '%', olc::RED);

        // Juego terminado
        if (bDead) {
            std::string sText = "PRESS <SPACE> TO PLAY AGAIN";
            CGE_DrawString((ScreenWidth() - sText.length()) / 2, ScreenHeight() / 2, sText, olc::MAGENTA);
            if (GetKey(olc::Key::SPACE).bPressed) {
                bDead = false;
                ResetGame();
            }
        }

        return true;
    }
};

#define PIX_X 1
#define PIX_Y 2

int main()
{
    Snake_PGE game;
    if (game.Construct(nScreenWidth * 8, nScreenHeight * 8, PIX_X, PIX_Y))
        game.Start();

    return 0;
}