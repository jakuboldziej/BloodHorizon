#include "views/GameLoop.h"

#include "GameConfig.h"
#include "managers/CollisionManager.h"
#include "managers/InputManager.h"

GameLoop::GameLoop() {
  player1 = std::make_unique<Player>(true);
  player2 = std::make_unique<Player>(false);

  player1->playIdleAnimation();
  player2->playIdleAnimation();

  CollisionManager &collisionManager = CollisionManager::getInstance();
  SDL_FRect worldBounds = {0, 0, GameConfig::LOGICAL_WIDTH, GameConfig::LOGICAL_HEIGHT};
  collisionManager.setWorldBounds(worldBounds);

  collisionManager.setOnPlayerHitCallback([this](Player *attacker, Player *defender) {
    // sound effects, particles
  });

  collisionManager.setOnBoundaryHitCallback([this](Player *player) {
    // prevent movement
  });
}

GameLoop::~GameLoop() {
  player1.reset();
  player2.reset();
}

bool GameLoop::update(InputManager inputManager, float deltaTime) {
  handleInput(inputManager);

  player1->update(deltaTime);
  player2->update(deltaTime);

  CollisionManager &collisionManager = CollisionManager::getInstance();

  collisionManager.checkPlayerCollisions(player1.get(), player2.get());

  collisionManager.checkPlayerBoundaryCollisions(player1.get());
  collisionManager.checkPlayerBoundaryCollisions(player2.get());

  collisionManager.checkPlayerAttackCollisions(player1.get(), player2.get());
  collisionManager.checkPlayerAttackCollisions(player2.get(), player1.get());

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