#ifndef SPRITES_EXTRA_H
#define SPRITES_EXTRA_H

#include "common.h"

#include <chrono>
#include <thread>
#include <GL/freeglut.h>


void RenderBitmapText(const char* string, float x, float y, void* font) {
    glRasterPos3f(x, y, 0.0);  // Add 0.0 as the third argument
    for (const char* c = string; *c != '\0'; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

void RenderMenuOptions() {
    // Render game title
    glColor3f(1.0, 1.0, 1.0); // Set text color to white
    const char* title = "SPACE SWEEPER";
    int titleWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(title));
    RenderBitmapText(title, (glutGet(GLUT_WINDOW_WIDTH) - titleWidth) / 2, glutGet(GLUT_WINDOW_HEIGHT) - 60, GLUT_BITMAP_HELVETICA_18);

    // Render instructions
    const char* instructions1 = "Press [ENTER] to begin";
    int instructionsWidth1 = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(instructions1));
    RenderBitmapText(instructions1, (glutGet(GLUT_WINDOW_WIDTH) - instructionsWidth1) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2, GLUT_BITMAP_HELVETICA_18);

    const char* instructions2 = "or [Q] to quit";
    int instructionsWidth2 = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(instructions2));
    RenderBitmapText(instructions2, (glutGet(GLUT_WINDOW_WIDTH) - instructionsWidth2) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2 - 30, GLUT_BITMAP_HELVETICA_18);
}

void RenderGameOver() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1.0, 0.0, 0.0); // Set text color to red for game over
    const char* gameOverText = "Game Over!";
    int gameOverWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(gameOverText));
    RenderBitmapText(gameOverText, (glutGet(GLUT_WINDOW_WIDTH) - gameOverWidth) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2, GLUT_BITMAP_HELVETICA_18);

    glColor3f(1.0, 1.0, 1.0);
    const char* restartText = "Press [R] to restart";
    int restartWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(restartText));
    RenderBitmapText(restartText, (glutGet(GLUT_WINDOW_WIDTH) - restartWidth) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2 - 30, GLUT_BITMAP_HELVETICA_18);
}

void RenderGameWin() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.0, 1.0, 0.0); // Set text color to green for game win
    const char* winText = "Congratulations, You Win!";
    int winWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(winText));
    RenderBitmapText(winText, (glutGet(GLUT_WINDOW_WIDTH) - winWidth) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2, GLUT_BITMAP_HELVETICA_18);

    glColor3f(1.0, 1.0, 1.0);
    const char* restartText = "Press [R] to restart";
    int restartWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(restartText));
    RenderBitmapText(restartText, (glutGet(GLUT_WINDOW_WIDTH) - restartWidth) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2 - 30, GLUT_BITMAP_HELVETICA_18);
}

void RenderScore(unsigned int score) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1.0, 1.0, 1.0); // Set text color to white for score
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    RenderBitmapText(scoreText, 10, glutGet(GLUT_WINDOW_HEIGHT) - 20, GLUT_BITMAP_HELVETICA_18);
}

void RenderCurrentLevel(int currentLevel) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1.0, 1.0, 1.0); // Set text color to white for level
    char levelText[50];
    snprintf(levelText, sizeof(levelText), "Level: %d", currentLevel);
    RenderBitmapText(levelText, glutGet(GLUT_WINDOW_WIDTH) - 75, glutGet(GLUT_WINDOW_HEIGHT) - 20, GLUT_BITMAP_HELVETICA_18);
}

void RenderLevelUp() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.0, 1.0, 0.0); // Set text color to green for level up message
    const char* levelUpText = "Level Up...";
    int levelUpWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>(levelUpText));
    RenderBitmapText(levelUpText, (glutGet(GLUT_WINDOW_WIDTH) - levelUpWidth) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2, GLUT_BITMAP_HELVETICA_18);

    glutSwapBuffers();

    // Pause for 1.5 seconds (adjust the duration as needed)
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Clear the level up message by redrawing the scene
    glutPostRedisplay();
}

#endif // SPRITES_EXTRA_H