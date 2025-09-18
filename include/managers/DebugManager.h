#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <string>
#include <vector>

#include "managers/FontManager.h"
#include "managers/InputManager.h"

class Player;

class DebugManager {
 public:
  static DebugManager &getInstance();

  void initialize(SDL_Renderer *renderer);
  void setDebugMode(bool enabled) { debugMode = enabled; }
  bool isDebugMode() const { return debugMode; }

  void render(SDL_Renderer *renderer);
  void clear();

  void debugPlayer(const Player *player, const std::string &playerName);
  void debugInputManager(const InputManager &inputManager);
  void debugGameState(int gameState);
  void debugCursorPosition(float x, float y);
  void debugCollisionManager();

  void addDebugText(const std::string &text);
  void addDebugValue(const std::string &name, float value);
  void addDebugValue(const std::string &name, int value);
  void addDebugValue(const std::string &name, bool value);

  void renderCollisionBoxes(SDL_Renderer *renderer, const Player *player1, const Player *player2);

 private:
  DebugManager() = default;
  ~DebugManager() = default;
  DebugManager(const DebugManager &) = delete;
  DebugManager &operator=(const DebugManager &) = delete;

  bool debugMode = false;
  SDL_Renderer *renderer = nullptr;

  static constexpr int DEBUG_FONT_SIZE = 12;

  struct DebugLine {
    std::string text;
    int yOffset;
  };

  std::vector<DebugLine> debugLines;
  int currentYOffset = 5;

  void addLine(const std::string &text);
  void resetYOffset() { currentYOffset = 5; }
};
