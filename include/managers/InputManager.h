#pragma once

#include <SDL3/SDL.h>

#include <bitset>
#include <unordered_map>

#include "glm/glm.hpp"

enum class PlayerAction {
  MoveLeft = 0,
  MoveRight,
  Jump,
  Punch,
  COUNT  // total number of actions
};

enum class PlayerId {
  Player1 = 0,
  Player2,
  COUNT
};

class InputManager {
 public:
  bool primary = false;

  void initProcessSession();
  void processEvent(SDL_Event event);

  bool isActionPressed(PlayerId playerId, PlayerAction action) const;

  bool isKeyPressing(PlayerId playerId) const;

  glm::vec2 getCursorPosition(SDL_Renderer *renderer) const;

 private:
  std::bitset<static_cast<size_t>(PlayerAction::COUNT)> playerStates[static_cast<size_t>(PlayerId::COUNT)];

  // Key map
  static const std::unordered_map<SDL_Keycode, std::pair<PlayerId, PlayerAction>> keyMappings;

  void setActionState(PlayerId playerId, PlayerAction action, bool isPressed);
};