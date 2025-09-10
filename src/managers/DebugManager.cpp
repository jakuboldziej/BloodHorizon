#include "managers/DebugManager.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "Player.h"

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

void DebugManager::addLine(const std::string &text) {
  debugLines.push_back({text, currentYOffset});
  currentYOffset += 10;
}
