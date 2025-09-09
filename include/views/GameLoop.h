#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "Player.h"
#include "managers/ResourceManager.h"

class GameLoop {
 public:
  GameLoop();
  ~GameLoop();

  bool update();
  void render(SDL_Renderer *renderer);

 private:
  std::unique_ptr<Player> player1 = nullptr;
  std::unique_ptr<Player> player2 = nullptr;
};