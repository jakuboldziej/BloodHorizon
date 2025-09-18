#include "managers/DebugManager.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "Player.h"
#include "managers/CollisionManager.h"

DebugManager &DebugManager::getInstance() {
  static DebugManager instance;
  return instance;
}

void DebugManager::initialize(SDL_Renderer *renderer) {
  this->renderer = renderer;
}

void DebugManager::render(SDL_Renderer *renderer) {
  if (!debugMode || !renderer)
    return;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  for (const auto &line : debugLines) {
    SDL_RenderDebugText(renderer, 5, line.yOffset, line.text.c_str());
  }
}

void DebugManager::clear() {
  debugLines.clear();
  resetYOffset();
}

void DebugManager::debugPlayer(const Player *player, const std::string &playerName) {
  if (!debugMode || !player)
    return;

  addLine("=== " + playerName + " DEBUG ===");

  auto pos = player->getPosition();
  addLine(playerName + " Pos: (" + std::to_string((int)pos.x) + ", " + std::to_string((int)pos.y) + ")");

  int currentAnim = player->getCurrentAnimation();
  std::string animName = "Unknown";
  switch (currentAnim) {
    case 0:
      animName = "Idle";
      break;
    case 1:
      animName = "Run";
      break;
    case 2:
      animName = "Punch";
      break;
  }

  addLine(playerName + " Animation: " + animName + " (" + std::to_string(currentAnim) + ")");
  addLine(playerName + " Animation: " + animName + " (" + std::to_string(currentAnim) + ")");
  addLine(playerName + " Is grounded: " + (player->isPlayerGrounded() ? "Yes" : "No"));

  bool isMoving = player->isMoving();
  addLine(playerName + " Moving: " + (isMoving ? "YES" : "NO"));

  addLine("");
}

void DebugManager::debugInputManager(const InputManager &inputManager) {
  if (!debugMode)
    return;

  addLine("=== INPUT DEBUG ===");

  // Player 1 input states
  std::string p1Input = "P1: ";
  if (inputManager.isActionPressed(PlayerId::Player1, PlayerAction::MoveLeft))
    p1Input += "LEFT ";
  if (inputManager.isActionPressed(PlayerId::Player1, PlayerAction::MoveRight))
    p1Input += "RIGHT ";
  if (inputManager.isActionPressed(PlayerId::Player1, PlayerAction::Jump))
    p1Input += "JUMP ";
  if (inputManager.isActionPressed(PlayerId::Player1, PlayerAction::Punch))
    p1Input += "PUNCH ";
  if (p1Input == "P1: ")
    p1Input += "NONE";
  addLine(p1Input);

  // Player 2 input states
  std::string p2Input = "P2: ";
  if (inputManager.isActionPressed(PlayerId::Player2, PlayerAction::MoveLeft))
    p2Input += "LEFT ";
  if (inputManager.isActionPressed(PlayerId::Player2, PlayerAction::MoveRight))
    p2Input += "RIGHT ";
  if (inputManager.isActionPressed(PlayerId::Player2, PlayerAction::Jump))
    p2Input += "JUMP ";
  if (inputManager.isActionPressed(PlayerId::Player2, PlayerAction::Punch))
    p2Input += "PUNCH ";
  if (p2Input == "P2: ")
    p2Input += "NONE";
  addLine(p2Input);

  addLine("");
}

void DebugManager::debugGameState(int gameState) {
  if (!debugMode)
    return;

  addLine("=== GAME DEBUG ===");
  addLine("Game State: " + std::to_string(gameState));
}

void DebugManager::debugCursorPosition(float x, float y) {
  if (!debugMode)
    return;

  std::string posStr = "Cursor: (" + std::to_string((int)x) + ", " + std::to_string((int)y) + ")";
  addLine(posStr);
  addLine("");
}

void DebugManager::addDebugText(const std::string &text) {
  if (!debugMode)
    return;
  addLine(text);
}

void DebugManager::addDebugValue(const std::string &name, float value) {
  if (!debugMode)
    return;

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << value;
  addLine(name + ": " + oss.str());
}

void DebugManager::addDebugValue(const std::string &name, int value) {
  if (!debugMode)
    return;
  addLine(name + ": " + std::to_string(value));
}

void DebugManager::addDebugValue(const std::string &name, bool value) {
  if (!debugMode)
    return;
  addLine(name + ": " + (value ? "TRUE" : "FALSE"));
}

void DebugManager::debugCollisionManager() {
  if (!debugMode)
    return;

  CollisionManager &collisionManager = CollisionManager::getInstance();

  addLine("=== COLLISION DEBUG ===");

  SDL_FRect bounds = collisionManager.getWorldBounds();
  addLine("World Bounds: (" + std::to_string((int)bounds.x) + ", " +
          std::to_string((int)bounds.y) + ", " +
          std::to_string((int)bounds.w) + ", " +
          std::to_string((int)bounds.h) + ")");

  auto collisions = collisionManager.getLastFrameCollisions();
  addLine("Collisions this frame: " + std::to_string(collisions.size()));

  for (size_t i = 0; i < collisions.size() && i < 3; ++i) {  // Show max 3 collisions
    const auto &collision = collisions[i];
    std::string typeStr = "Unknown";

    switch (collision.type) {
      case CollisionType::PLAYER_BOUNDARY:
        typeStr = "Boundary";
        break;
      case CollisionType::PLAYER_VS_PLAYER:
        typeStr = "Player vs Player";
        break;
      case CollisionType::ATTACK_HIT:
        typeStr = "Attack Hit";
        break;
      default:
        break;
    }

    addLine("  " + std::to_string(i + 1) + ": " + typeStr +
            " at (" + std::to_string((int)collision.contactPoint.x) +
            ", " + std::to_string((int)collision.contactPoint.y) + ")");
  }

  addLine("");
}

void DebugManager::renderCollisionBoxes(SDL_Renderer *renderer, const Player *player1, const Player *player2) {
  if (!debugMode || !renderer)
    return;

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  if (player1) {
    SDL_FRect hitbox1 = player1->getWorldHitbox();
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
    SDL_RenderFillRect(renderer, &hitbox1);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderRect(renderer, &hitbox1);

    if (player1->getCurrentAnimation() == 2) {
      SDL_FRect attackBox1 = player1->getAttackBox();
      if (attackBox1.w > 0 && attackBox1.h > 0) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);
        SDL_RenderFillRect(renderer, &attackBox1);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderRect(renderer, &attackBox1);
      }
    }
  }

  if (player2) {
    SDL_FRect hitbox2 = player2->getWorldHitbox();
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
    SDL_RenderFillRect(renderer, &hitbox2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderRect(renderer, &hitbox2);

    // Render attack box if punching
    if (player2->getCurrentAnimation() == 2) {
      SDL_FRect attackBox2 = player2->getAttackBox();
      if (attackBox2.w > 0 && attackBox2.h > 0) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);
        SDL_RenderFillRect(renderer, &attackBox2);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderRect(renderer, &attackBox2);
      }
    }
  }

  CollisionManager &collisionManager = CollisionManager::getInstance();
  SDL_FRect worldBounds = collisionManager.getWorldBounds();
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderRect(renderer, &worldBounds);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void DebugManager::addLine(const std::string &text) {
  debugLines.push_back({text, currentYOffset});
  currentYOffset += 10;
}
