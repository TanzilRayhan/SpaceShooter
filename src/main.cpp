/*
Project Name: Space Shooter
Course: Computer Graphics
Section: C
Course Teacher: Rahul Biswas
Semester: Fall 2023-24
Team members:
            Name                    ID
    - Md Sadman Shakib Alam     22-46262-1
    - Md Sabbir Sikder          22-46005-1
    - Tanzil Rayhan             22-46300-1
    - Toufiq Ahmed Shishir      22-46260-1
*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <thread>
#include <mmsystem.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "common.h"
#include "engine.h"
#include "sprites_extra.h"

#define WINDOW_TITLE_PREFIX "Space Shooter"

const unsigned int WIN_WIDTH = 680;
const unsigned int WIN_HEIGHT = 768;

const wchar_t* fire_sound = L"assets/Audio/laser3.wav";
const wchar_t* destroy_sound = L"assets/Audio/destroy.wav";
const wchar_t* game_over_sound = L"assets/Audio/lowThreeTone.wav";
const wchar_t* game_tone = L"assets/Audio/laser3.wav";
const wchar_t* game_win_sound = L"assets/Audio/game-win.wav";

struct Star {
    float x, y;
    float speed;

    Star(float x, float y, float speed) : x(x), y(y), speed(speed) {}
};

struct Pixmap {
    unsigned char* m_data;
    int m_width, m_height, m_channels;
    int x, y;
    float speed;
    int HP, DP;

    // NOTE: operator== overloaded for collision ckecking
    bool operator==(const Pixmap& other) const {
        return (
            m_data == other.m_data &&
            m_width == other.m_width &&
            m_height == other.m_height &&
            m_channels == other.m_channels &&
            x == other.x &&
            y == other.y &&
            speed == other.speed
        );
    }
};

// function declarations
void RenderScene();
void UpdateScene(int val);
void ReshapeScene(int width, int height);
std::vector<Pixmap> InitSprites();
void DeinitSprites(std::vector<Pixmap> sprite);
void RenderSprite(Pixmap sprite);
void CreateStarfield();
void UpdateStarfield();
void RenderStar(Star star);
void ShootLaser();
void UpdateLaser();
void CreateObstacles();
void UpdateObstacles();
bool CheckCollision(Pixmap& laser, Pixmap& obstacle);
void RenderSpriteSingle(Pixmap sprite);
// NOTE: bitmap text rendering functions
void RenderBitmapText(const char* string, float x, float y, void* font);
void RenderMenuOptions();
void ResetGame();
// NOTE: sound/audio output fucntions
void PlayLaserSound();
void PlayDestroySound();
void PlayWinSound();
void PlayGameOverSound();
void PlayGameTone();

// NOTE: global attributes declaration
Engine engine;

std::vector<Star>   stars;
std::vector<Pixmap> sprites;
std::vector<Pixmap> laser;
std::vector<Pixmap> obstacles;

int dur = 0;
unsigned int score = 0, curr_level = 1;
bool l2flag = true;
int obstacleInterval = 2000;
int starfieldInterval = 500;
int rand_count = 0;

// TODO: add blink effect on player

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow(WINDOW_TITLE_PREFIX);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(1);
    }

    engine.Init();
    sprites = InitSprites();

    glutDisplayFunc(RenderScene);
    glutTimerFunc(800, UpdateScene, 0);
    glutReshapeFunc(ReshapeScene);
    // <------ Event Handlers ------>
    // NOTE: player passive motion (mouse movement)
    glutPassiveMotionFunc([](int x, int y) {
        if (x - sprites[0].m_width / 2 < 0) {
            sprites[0].x = 0.0f;
        } else if (x - sprites[0].m_width / 2 > WIN_WIDTH - sprites[0].m_width) {
            sprites[0].x = (float)(WIN_WIDTH - sprites[0].m_width);
        } else {
            sprites[0].x = (float)(x - sprites[0].m_width / 2);
        }

        if (WIN_HEIGHT - y - sprites[0].m_height / 2 < 0) {
            sprites[0].y = 0.0f;
        } else if (WIN_HEIGHT - y - sprites[0].m_height / 2 > WIN_HEIGHT - sprites[0].m_height) {
            sprites[0].y = (float)(WIN_HEIGHT - sprites[0].m_height);
        } else {
            sprites[0].y = (float)(WIN_HEIGHT - y - sprites[0].m_height / 2);
        }
    });
    // NOTE: mouse click event handler
    glutMouseFunc([](int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            std::cout << "Left mouse click\n";
            std::thread(PlayLaserSound).detach();
            ShootLaser();
        }
    });
    // NOTE: keyboard events handler
    glutKeyboardFunc([](unsigned char key, int x, int y) {
        switch (key) {
            case 13: // ASCII code for ENTER key
                engine.UpdateState(GameState::IN_GAME);
                break;
            case 27: // ASCII code for ESC key
                engine.UpdateState(GameState::IN_MENU);
                break;
            case 'Q':
            case 'q':
                if (engine.GetState() == GameState::IN_MENU) exit(0);
                break;
            case 'R':
            case 'r':
                if (engine.GetState() == GameState::GAME_OVER or
                    engine.GetState() == GameState::GAME_WIN) {
                    engine.Init();
                    sprites = InitSprites();
                    obstacles.clear();
                    score = 0;
                    engine.UpdateState(GameState::IN_MENU);
                    if (engine.GetState() == GameState::GAME_OVER) 
                        std::thread(PlayGameOverSound).detach();
                    else if (engine.GetState() == GameState::GAME_WIN) 
                        std::thread(PlayWinSound).detach();
                }
                break;
        }
    });
    
    GLCall(glClearColor(0.0, 0.0, 0.0, 1.0));

    GLCall(glMatrixMode(GL_PROJECTION));
    GLCall(glLoadIdentity());
    GLCall(gluOrtho2D(0.0, 680.0, 0.0, 768.0));
    GLCall(glMatrixMode(GL_MODELVIEW));

    glutIdleFunc([]() {
        if (engine.currState == GameState::IN_GAME){
            UpdateObstacles();
        }
        UpdateStarfield();
        glutPostRedisplay();
    });

    glutSetCursor(GLUT_CURSOR_NONE);

    // std::thread(PlayGameTone).detach();

    glutMainLoop();
    
    DeinitSprites(sprites);
    return 0;
}

void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (engine.GetState() == GameState::IN_MENU or
        engine.GetState() == GameState::IN_GAME or
        engine.GetState() == GameState::GAME_OVER or
        engine.GetState() == GameState::GAME_WIN) {
        // NOTE: render obstacles and lasers
        RenderSprite(sprites[0]);
        RenderScore(score);
        RenderCurrentLevel(curr_level);

        if (dur == 2000) dur = 0, CreateObstacles();

        // NOTE: render player life indicators at the top-right corner
        for (int i = 0; i < sprites[0].HP; ++i) {
            RenderSpriteSingle(sprites[3]);
            glTranslatef(sprites[3].m_width + 5, 0.0, 0.0); // Move to the next position
        }
        glLoadIdentity(); // Reset transformation matrix

        for (auto& lsr : laser) {
            glRasterPos2i(lsr.x, lsr.y);
            glDrawPixels(
                lsr.m_width,
                lsr.m_height,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                lsr.m_data
            );
        }

        for (auto& obstacle : obstacles) {
            glRasterPos2i(obstacle.x, obstacle.y);
            glDrawPixels(
                obstacle.m_width,
                obstacle.m_height,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                obstacle.m_data
            );
        }

        CreateStarfield();
        for (auto& star : stars) RenderStar(star);
    }

    if (engine.currState == GameState::IN_MENU)     RenderMenuOptions();
    if (engine.currState == GameState::GAME_OVER)   RenderGameOver();
    if (engine.currState == GameState::GAME_WIN)    RenderGameWin();

    glutSwapBuffers();
    dur += 25;
}

void UpdateScene(int val) {
    // updates laser
    UpdateLaser();

    // NOTE: check for collisions
    for (auto it = laser.begin(); it != laser.end(); ) {
        for (auto& obstacle : obstacles) {
            if (CheckCollision(*it, obstacle)) {
                it = laser.erase(it);

                // NOTE: check obstacle HP
                if (obstacle.HP <= 0) {
                    obstacles.erase(std::remove(obstacles.begin(), obstacles.end(), obstacle), obstacles.end());
                    score += 3;
                    std::thread(PlayDestroySound).detach();
                }
                break;
            }
        }
        if (it != laser.end()) {
            ++it;
        }
    }

    // NOTE: check player collision with obstacles
    for (auto& obstacle : obstacles) {
        if (CheckCollision(sprites[0], obstacle)) {
            obstacles.erase(std::remove(obstacles.begin(), obstacles.end(), obstacle), obstacles.end());
            sprites[0].HP -= 1;
            std::thread(PlayDestroySound).detach();

            // Check player HP
            if (sprites[0].HP <= 0) {
                // Handle player defeat
                std::cout << "Game Over!\n";
                engine.currState = GameState::GAME_OVER;
                // Reset the game when game over
                ResetGame();
                // engine.UpdateState(GameState::IN_MENU);
            } /* else {
                blinkPlayer = true;
                blinkCount = 0;
            } */
        }
    }

    if (sprites[0].HP <= 0 and
        engine.currState != GameState::GAME_WIN) 
        engine.UpdateState(GameState::GAME_OVER);
    
    if (score >= 60) {
        engine.UpdateState(GameState::GAME_WIN);
        ResetGame();
    }

    glutPostRedisplay();
    glutTimerFunc(35, UpdateScene, 0);
}

void ReshapeScene(int width, int height) {
    glutReshapeWindow(width, height);
}

std::vector<Pixmap> InitSprites() {
    std::vector<Pixmap> sprite(5); // index 0 -> player, index 1 -> laser, index 2 -> obstacle, index 3 -> heart

    stbi_set_flip_vertically_on_load(true);
    // load player
    sprite[0].m_data = stbi_load("assets/PNG/playerShip1_red.bmp", &sprite[0].m_width, &sprite[0].m_height, &sprite[0].m_channels, 0);
    if (!sprite[0].m_data) {
        fprintf(stderr, "Error loading BMP image: %s\n", stbi_failure_reason());
        exit(1);
    }
    sprite[0].x = WIN_WIDTH/2 - sprite[0].m_width/2;
    sprite[0].y = 100;
    sprite[0].speed = 0;
    sprite[0].HP = 3;
    sprite[0].DP = 20;
    // load laser
    sprite[1].m_data = stbi_load("assets/PNG/Lasers/laserRed03.bmp", &sprite[1].m_width, &sprite[1].m_height, &sprite[1].m_channels, 0);
    if (!sprite[1].m_data) {
        fprintf(stderr, "Error loading BMP image: %s\n", stbi_failure_reason());
        exit(1);
    }
    sprite[1].x = 0;
    sprite[1].y = 0;
    sprite[1].speed = 0.0;
    sprite[1].HP = 0;
    sprite[1].DP = 10;
    // load obstacle
    sprite[2].m_data = stbi_load("assets/PNG/Meteors/meteorGrey_med2.bmp", &sprite[2].m_width, &sprite[2].m_height, &sprite[2].m_channels, 0);
    if (!sprite[2].m_data) {
        fprintf(stderr, "Error loading BMP image: %s\n", stbi_failure_reason());
        exit(1);
    }
    sprite[2].x = 0;
    sprite[2].y = 0;
    sprite[2].speed = 2;
    sprite[2].HP = 20;
    sprite[2].DP = 34;

    // load player life
    sprite[3].m_data = stbi_load("assets/PNG/UI/playerLife1_red.bmp", &sprite[3].m_width, &sprite[3].m_height, &sprite[3].m_channels, 0);
    if (!sprite[3].m_data) {
        fprintf(stderr, "Error loading BMP image: %s\n", stbi_failure_reason());
        exit(1);
    }
    sprite[3].x = 15;
    sprite[3].y = 10;
    sprite[3].speed = 0;
    sprite[3].HP = 0;
    sprite[3].DP = 0;

    // load player life
    sprite[4].m_data = stbi_load("assets/PNG/Meteors/meteorBrown_med1.bmp", &sprite[4].m_width, &sprite[4].m_height, &sprite[4].m_channels, 0);
    if (!sprite[4].m_data) {
        fprintf(stderr, "Error loading BMP image: %s\n", stbi_failure_reason());
        exit(1);
    }
    sprite[4].x = 15;
    sprite[4].y = 10;
    sprite[4].speed = 0;
    sprite[4].HP = 0;
    sprite[4].DP = 0;

    return sprite;
}

void DeinitSprites(std::vector<Pixmap> sprite) {
    for (auto data : sprite) {
        if (data.m_data) stbi_image_free(data.m_data);
    }
}

void RenderSprite(Pixmap sprite) {
    glRasterPos2f(sprite.x, sprite.y);
    glDrawPixels(sprite.m_width, sprite.m_height, GL_RGBA, GL_UNSIGNED_BYTE, sprite.m_data);

    for (auto& lsr : laser) {
        glRasterPos2i(lsr.x, lsr.y);
        glDrawPixels(
            lsr.m_width, 
            lsr.m_height, 
            GL_RGBA, 
            GL_UNSIGNED_BYTE, 
            lsr.m_data
        );
    }

    for (auto& obstacle : obstacles) {
        glRasterPos2i(obstacle.x, obstacle.y);
        glDrawPixels(
            obstacle.m_width,
            obstacle.m_height,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            obstacle.m_data
        );
    }
}

void ShootLaser() {
    Pixmap lsr = sprites[1];
    lsr.x = sprites[0].x + sprites[0].m_width / 2 - lsr.m_width / 2;
    lsr.y = sprites[0].y + sprites[0].m_height;
    lsr.speed = 50.0;
    laser.push_back(lsr);
}

void UpdateLaser() {
    // NOTE: update laser positions
    for (auto& lsr : laser) {
        lsr.y += lsr.speed;
    }
    // NOTE: remove bullets that have gone off-screen
    laser.erase(
        std::remove_if(
            laser.begin(), 
            laser.end(), 
            [](const auto& lsr) { return lsr.y > 768; }
        ), 
        laser.end()
    );
}

void CreateStarfield() {
    Star star(
        std::rand() % (WIN_WIDTH),
        WIN_HEIGHT,
        5
    );
    stars.push_back(star);
}

void RenderStar(Star star) {
    glPointSize(2.0);
    glBegin(GL_POINTS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(star.x, star.y);
    glEnd();
}

void UpdateStarfield() {
    for (auto& star : stars) {
        star.y -= star.speed;
    }
    // NOTE: remove star that have gone off-screen
    stars.erase(
        std::remove_if(
            stars.begin(),
            stars.end(),
            [](const auto& star) { return star.y <= 0; }
        ),
        stars.end()
    );
}

void CreateObstacles() {
    Pixmap obstacle = (rand_count & 1) ? sprites[2] : sprites[4];
    rand_count = std::rand() % 100;
    obstacle.x = std::rand() % (WIN_WIDTH - obstacle.m_width / 2);
    obstacle.y = 768 + rand() % 101;
    obstacle.speed = (curr_level == 2) ? 5 : 2;
    obstacles.push_back(obstacle);
}

void UpdateObstacles() {
    // NOTE: update obstacle positions
    for (auto& obstacle : obstacles) {
        obstacle.y -= obstacle.speed;

        // NOTE: increase obstacle speed when score is 50 or more
        if (score >= 30 and l2flag) {
            RenderLevelUp();
            curr_level += 1;
            l2flag = false;
        }
    }
    // NOTE: remove obstacle that have gone off-screen
    obstacles.erase(
        std::remove_if(
            obstacles.begin(),
            obstacles.end(),
            [](const auto& obstacle) { return obstacle.y <= 0; }
        ),
        obstacles.end()
    );
}

bool CheckCollision(Pixmap& laser, Pixmap& obstacle) {
    if (
        laser.x < obstacle.x + obstacle.m_width &&
        laser.x + laser.m_width > obstacle.x &&
        laser.y < obstacle.y + obstacle.m_height &&
        laser.y + laser.m_height > obstacle.y
    ) {
        obstacle.HP -= laser.DP;
        return true;
    }

    return false;
}

void RenderSpriteSingle(Pixmap sprite) {
    glRasterPos2f(sprite.x, sprite.y);
    glDrawPixels(sprite.m_width, sprite.m_height, GL_RGBA, GL_UNSIGNED_BYTE, sprite.m_data);
}

void ResetGame() {
    // NOTE: reset player
    sprites[0].x = WIN_WIDTH / 2 - sprites[0].m_width / 2;
    sprites[0].y = 100;
    sprites[0].speed = 0;
    sprites[0].HP = 3; // Reset player HP

    // NOTE: clear laser, obstacles, and reset score
    laser.clear();
    obstacles.clear();
    score = 0;
    curr_level = 1;

    // NOTE: reset game duration
    dur = 0;
}

void PlayLaserSound() {
    PlaySoundW(fire_sound, NULL, SND_FILENAME | SND_ASYNC);
}

void PlayDestroySound() {
    PlaySoundW(destroy_sound, NULL, SND_FILENAME | SND_ASYNC);
}

void PlayWinSound() {
    PlaySoundW(game_win_sound, NULL, SND_FILENAME | SND_ASYNC);
}

void PlayGameOverSound() {
    PlaySoundW(game_over_sound, NULL, SND_FILENAME | SND_ASYNC);
}

void PlayGameTone() {
    // Play the background sound in a loop
    while (true) {
        PlaySoundW(game_tone, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}