#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "managers/ResourceManager.h"
#include "utils/SDLDeleter.h"
#include "views/GameLoop.h"
#include "views/MainMenu.h"

enum class GameState {
  MAINMENU,
  GAMELOOP
};

class Game {
 public:
  Game();
  ~Game();

  void cleanup();
  void run();

 private:
  unique_window window;
  unique_renderer renderer;

  GameState currentGameState = GameState::MAINMENU;

  InputManager inputManager;

  std::unique_ptr<MainMenu> mainMenuView = nullptr;
  std::unique_ptr<GameLoop> gameLoopView = nullptr;

  bool debugMode = false;
  bool fullscreen = false;
  bool running = true;
  bool quit = false;

  void update(float deltaTime);

  void render();
  void renderUI();
  void renderDebug();

  void changeGameState(GameState);
};