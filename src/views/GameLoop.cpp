#include "views/GameLoop.h"

#include "managers/InputManager.h"

GameLoop::GameLoop() {
  player1 = std::make_unique<Player>(true);
  player2 = std::make_unique<Player>(false);

  player1->playIdleAnimation();
  player2->playIdleAnimation();
}

GameLoop::~GameLoop() {
  player1.reset();
  player2.reset();
}

bool GameLoop::update(InputManager inputManager, float deltaTime) {
  handleInput(inputManager);
  player1->update(deltaTime);
  player2->update(deltaTime);

  return true;
}

void GameLoop::handleInput(const InputManager &inputManager) {
  // Handle Player 1 input
  bool player1Moving = false;

  if (inputManager.isActionPressed(PlayerId::Player1, PlayerAction::MoveLeft)) {
    player1->move(-1.0f);
    player1Moving = true;
  } else if (inputManager.isActionPressed(PlayerId::Player1, PlayerAction::MoveRight)) {
    player1->move(1.0f);
    player1Moving = true;
  }

  if (inputManager.isActionPressed(PlayerId::Player1, PlayerAction::Jump)) {
    player1->jump();
  }
  if (inputManager.isActionPressed(PlayerId::Player1, PlayerAction::Punch)) {
    player1->punch();
  }

  if (!player1Moving) {
    player1->stopMoving();
  }

  // Handle Player 2 input
  bool player2Moving = false;

  if (inputManager.isActionPressed(PlayerId::Player2, PlayerAction::MoveLeft)) {
    player2->move(-1.0f);
    player2Moving = true;
  } else if (inputManager.isActionPressed(PlayerId::Player2, PlayerAction::MoveRight)) {
    player2->move(1.0f);
    player2Moving = true;
  }

  if (inputManager.isActionPressed(PlayerId::Player2, PlayerAction::Jump)) {
    player2->jump();
  }
  if (inputManager.isActionPressed(PlayerId::Player2, PlayerAction::Punch)) {
    player2->punch();
  }

  if (!player2Moving) {
    player2->stopMoving();
  }
}

void GameLoop::render(SDL_Renderer *renderer) {
  player1->render(renderer);
  player2->render(renderer);
}