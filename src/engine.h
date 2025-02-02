#ifndef ENGINE_H
#define ENGINE_H

typedef enum {
    IN_GAME, IN_MENU, GAME_WIN, GAME_OVER, EXIT
} GameState;

struct Engine {
    GameState currState;

    void Init() { currState = GameState::IN_MENU; }
    void UpdateState(GameState newState) { currState = newState; }
    GameState GetState() { return currState; };
};

#endif // ENGINE_H