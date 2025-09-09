#include "views/GameLoop.h"

GameLoop::GameLoop() {
  player1 = std::make_unique<Player>(true);
  player2 = std::make_unique<Player>(false);
}

GameLoop::~GameLoop() {
  player1.reset();
  player2.reset();
}

bool GameLoop::update() {
  player1->update();
  player2->update();

  return true;
}

void GameLoop::render(SDL_Renderer *renderer) {
  player1->render(renderer);
  player2->render(renderer);
}