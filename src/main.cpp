#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <chrono>
#include <iostream>

class Paddle
{
public:
    Paddle() : x(0), y(0), width(0), height(0) {}

    Paddle(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

    void move_up(int speed)
    {
        if (y > 0)
        {
            y -= speed;
        }
    }

    void move_down(int speed)
    {
        if (y + height < screenHeight)
        {
            y += speed;
        }
    }

    int getPos()
        const
    {
        return x, y;
    }

    int getSize()
        const
    {
        return width, height;
    }

    void setPosY(int posY)
    {
        y = posY;
    }

    int getWidth() const
    {
        return width;
    }

    int getHeight() const
    {
        return height;
    }

    SDL_Rect getRect() const
    {
        return {x, y, width, height};
    }

private:
    int x, y, width, height;
    static const int screenWidth = 800;
    static const int screenHeight = 600;
};

class Ball
{
public:
    Ball() : x(0), y(0), size(0) {}

    Ball(int x, int y, int size) : x(x), y(y), size(size), ballSpeed(4), ballXDir(1), ballYDir(1) {}

    void move(Paddle rightPaddle, Paddle leftPaddle)
    {
        int leftPaddleY, leftPaddleX = leftPaddle.getPos();
        int rightPaddleY, rightPaddleX = rightPaddle.getPos();
        int paddleWidth, paddleHeight = leftPaddle.getSize();

        // Make ball move at defined speed
        x += ballXDir * ballSpeed;
        y += ballYDir * ballSpeed;

        // Collision detection with walls
        if (y < 0 || y + size > screenHeight)
        {
            ballYDir = -ballYDir;
        }

        // Make ball bounce if collide with a paddle
        if (checkPaddleCollision(rightPaddle, leftPaddle))
        {
            ballXDir = -ballXDir;
        }

        // Respawn the ball to the center if it goes out of bounds
        if (x < 0)
        {
            rightPlayerPoints++;
            x = (screenWidth - size) / 2;
            y = (screenHeight - size) / 2;
        }
        else if (x + size > screenWidth)
        {
            leftPlayerPoints++;
            x = (screenWidth - size) / 2;
            y = (screenHeight - size) / 2;
        }
    }

    int getPos()
        const
    {
        return x, y;
    }

    int getSize()
        const
    {
        return size;
    }

    int getDir()
        const
    {
        return ballXDir, ballYDir;
    }

    void setPos(int posX, int posY)
    {
        x = posX;
        y = posY;
    }

    void setDir(int xDir, int yDir)
    {
        ballXDir = xDir;
        ballYDir = yDir;
    }

    void setScore(int leftScore, int rightScore)
    {
        leftPlayerPoints = leftScore;
        rightPlayerPoints = rightScore;
    }

    int getLeftPlayerPoints() const
    {
        return leftPlayerPoints;
    }

    int getRightPlayerPoints() const
    {
        return rightPlayerPoints;
    }

    static int getScreenWidth()
    {
        return screenWidth;
    }

    static int getScreenHeight()
    {
        return screenHeight;
    }

    SDL_Rect getRect() const
    {
        return {x, y, size, size};
    }

private:
    int x, y, size, ballSpeed, ballXDir, ballYDir;
    static const int screenWidth = 800;
    static const int screenHeight = 600;
    int leftPlayerPoints = 0;
    int rightPlayerPoints = 0;

    bool checkPaddleCollision(Paddle rightPaddle, Paddle leftPaddle)
    {
        // Check if the ball intersects with the paddle
        SDL_Rect ballRect = getRect();
        SDL_Rect rightPaddleRect = rightPaddle.getRect();
        SDL_Rect leftPaddleRect = leftPaddle.getRect();

        return SDL_HasIntersection(&ballRect, &rightPaddleRect) || SDL_HasIntersection(&ballRect, &leftPaddleRect);
    }
};

class PongGame
{
public:
    PongGame()
    {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();

        frameCount = 0;
        totalTime = 0.0;
        fps = 0.0;

        const int window_width = 800;
        const int window_height = 600;

        // Create the SDL window
        window = SDL_CreateWindow(
            "Pong",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            window_width, window_height,
            SDL_WINDOW_SHOWN);

        // Init SDL renderer and TTF font
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        font = TTF_OpenFont("../\\font\\Inter-Bold.ttf", 24); // Replace with the path to your TTF font file

        int paddleWidth = 20;
        int paddleHeight = 100;

        // Init the ball and the right and left paddles
        rightPaddle = Paddle(window_width - paddleWidth, ((window_height - paddleHeight) / 2), paddleWidth, paddleHeight);
        leftPaddle = Paddle(0, ((window_height - paddleHeight) / 2), paddleWidth, paddleHeight);
        ball = Ball(400, 300, 20);
    }

    ~PongGame()
    {
        // Cleanup resources
        TTF_CloseFont(font);
        TTF_Quit();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    int getWinSize()
    {
        return window_width, window_height;
    }

    // Main game loop
    void run()
    {
        using namespace std::chrono;
        auto lastFrameTime = high_resolution_clock::now();

        bool quit = false;
        SDL_Event event;

        while (!quit)
        {
            auto currentFrameTime = high_resolution_clock::now();
            auto deltaTime = duration_cast<milliseconds>(currentFrameTime - lastFrameTime).count() / 1000.0;

            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    // Handle keyboard events
                    switch (event.key.keysym.sym)
                    {
                    // "P" key to pause the game
                    case SDLK_p:
                        if (isPaused == false)
                        {
                            isPaused = true;
                        }
                        else if (isPaused == true)
                        {
                            isPaused = false;
                        }
                        break;
                    // "R" key to reset the game
                    case SDLK_r:
                        if (isPaused == true)
                        {
                            isPaused = false;
                        }

                        int ballSize = ball.getSize();
                        const int paddleWidth = leftPaddle.getWidth();
                        const int paddleHeight = leftPaddle.getHeight();

                        ball.setScore(0, 0);
                        ball.setPos((ball.getScreenWidth() - ballSize) / 2, (ball.getScreenWidth() - ballSize) / 2);
                        ball.setDir(1, 1);

                        leftPaddle.setPosY((ball.getScreenWidth() / 2) - (paddleHeight + (paddleHeight / 2)));
                        rightPaddle.setPosY((ball.getScreenWidth() / 2) - (paddleHeight + (paddleHeight / 2)));
                    }
                    break;
                }
            }

            handleInput();
            update(deltaTime);
            render();

            lastFrameTime = currentFrameTime;
        }
    }

private:
    int paddleWidth, paddleHeight, window_width, window_height, frameCount;
    double totalTime, fps;
    bool isPaused = false;
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    Paddle rightPaddle;
    Paddle leftPaddle;
    Ball ball;

    // Handle input to control left paddle
    void handleInput()
    {
        const int paddleSpeed = 10;

        const Uint8 *state = SDL_GetKeyboardState(NULL);

        int leftPaddleY, leftPaddleX = leftPaddle.getPos();
        int rightPaddleY, rightPaddleX = rightPaddle.getPos();

        if (!isPaused)
        {
            if (state[SDL_SCANCODE_W])
            {
                leftPaddle.move_up(paddleSpeed);
            }
            else if (state[SDL_SCANCODE_S])
            {
                leftPaddle.move_down(paddleSpeed);
            }
        }
    }

    void update(double deltaTime)
    {
        if (!isPaused)
        {
            // Update game logic
            ball.move(rightPaddle, leftPaddle);

            const int paddleSpeed = 15;             // Adjust this value to control the speed of the paddles
            const double interpolationFactor = 0.3; // Adjust this value to control the smoothness (0.0 for instant, 1.0 for no change)

            int ballCenterY = ball.getRect().y + ball.getRect().h / 2;
            int rightPaddleCenterY = rightPaddle.getRect().y + rightPaddle.getRect().h / 2;

            // Basic AI opponent
            if (ballCenterY < rightPaddleCenterY)
            {
                rightPaddle.move_up(paddleSpeed * interpolationFactor);
            }
            else if (ballCenterY > rightPaddleCenterY)
            {
                rightPaddle.move_down(paddleSpeed * interpolationFactor);
            }
        }

        // Calculate FPS
        frameCount++;
        totalTime += deltaTime;
        if (totalTime > 1.0)
        {
            fps = frameCount / totalTime;
            frameCount = 0;
            totalTime = 0.0;
        }
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (!isPaused)
        {
            // Render paddle
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            SDL_Rect leftPaddleRect = leftPaddle.getRect();
            SDL_Rect rightPaddleRect = rightPaddle.getRect();

            SDL_RenderFillRect(renderer, &leftPaddleRect);
            SDL_RenderFillRect(renderer, &rightPaddleRect);

            // Render ball
            SDL_Rect ballRect = ball.getRect();
            SDL_RenderFillRect(renderer, &ballRect);

            // Display score
            renderText(std::to_string(ball.getLeftPlayerPoints()) + " - " + std::to_string(ball.getRightPlayerPoints()), ball.getScreenWidth() / 2 - 50, 20);

            // Display FPS
            renderText("FPS: " + std::to_string(static_cast<int>(fps)), 10, 10);
        }
        else
        {
            // Display pause mode text
            renderText("PAUSED", ball.getScreenWidth() / 2 - 50, ball.getScreenHeight() / 2 - 10);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // 16 milliseconds corresponds to approximately 60 frames per second
    }

    // Render text using TTF
    void renderText(const std::string &text, int x, int y)
    {
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        int textWidth = textSurface->w;
        int textHeight = textSurface->h;

        SDL_Rect textRect = {x, y, textWidth, textHeight};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
};

int main()
{
    PongGame game;
    game.run();

    return 0;
}
