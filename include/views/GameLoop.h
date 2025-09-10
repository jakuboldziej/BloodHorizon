#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "Player.h"
#include "managers/InputManager.h"
#include "managers/ResourceManager.h"

class GameLoop {
 public:
  GameLoop();
  ~GameLoop();

  bool update(InputManager inputManager, float deltaTime);
  void handleInput(const InputManager &inputManager);
  void render(SDL_Renderer *renderer);

  const Player *getPlayer1() const { return player1.get(); }
  const Player *getPlayer2() const { return player2.get(); }

 private:
  std::unique_ptr<Player> player1 = nullptr;
  std::unique_ptr<Player> player2 = nullptr;
};