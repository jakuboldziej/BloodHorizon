#include "managers/InputManager.h"

const std::unordered_map<SDL_Keycode, std::pair<PlayerId, PlayerAction>> InputManager::keyMappings = {
    // Player 1 controls (WASD + Space)
    {SDLK_A, {PlayerId::Player1, PlayerAction::MoveLeft}},
    {SDLK_D, {PlayerId::Player1, PlayerAction::MoveRight}},
    {SDLK_W, {PlayerId::Player1, PlayerAction::Jump}},
    {SDLK_SPACE, {PlayerId::Player1, PlayerAction::Punch}},

    // Player 2 controls (Arrow keys + Enter)
    {SDLK_LEFT, {PlayerId::Player2, PlayerAction::MoveLeft}},
    {SDLK_RIGHT, {PlayerId::Player2, PlayerAction::MoveRight}},
    {SDLK_UP, {PlayerId::Player2, PlayerAction::Jump}},
    {SDLK_RETURN, {PlayerId::Player2, PlayerAction::Punch}}};

void InputManager::initProcessSession() {
  primary = false;
}

void InputManager::processEvent(SDL_Event event) {
  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      primary = true;
  }

  if (event.type == SDL_EVENT_KEY_DOWN) {
    auto it = keyMappings.find(event.key.key);
    if (it != keyMappings.end()) {
      PlayerId playerId = it->second.first;
      PlayerAction action = it->second.second;
      setActionState(playerId, action, true);
    }
  } else if (event.type == SDL_EVENT_KEY_UP) {
    auto it = keyMappings.find(event.key.key);
    if (it != keyMappings.end()) {
      PlayerId playerId = it->second.first;
      PlayerAction action = it->second.second;
      setActionState(playerId, action, false);
    }
  }
}

bool InputManager::isActionPressed(PlayerId playerId, PlayerAction action) const {
  int playerIndex = static_cast<int>(playerId);
  int actionIndex = static_cast<int>(action);

  if (playerIndex >= 0 && playerIndex < static_cast<int>(PlayerId::COUNT) &&
      actionIndex >= 0 && actionIndex < static_cast<int>(PlayerAction::COUNT)) {
    return playerStates[playerIndex][actionIndex];
  }

  return false;
}

bool InputManager::isKeyPressing(PlayerId playerId) const {
  return isActionPressed(playerId, PlayerAction::MoveLeft) ||
         isActionPressed(playerId, PlayerAction::MoveRight) ||
         isActionPressed(playerId, PlayerAction::Jump);
}

void InputManager::setActionState(PlayerId playerId, PlayerAction action, bool isPressed) {
  int playerIndex = static_cast<int>(playerId);
  int actionIndex = static_cast<int>(action);

  if (playerIndex >= 0 && playerIndex < static_cast<int>(PlayerId::COUNT) &&
      actionIndex >= 0 && actionIndex < static_cast<int>(PlayerAction::COUNT)) {
    playerStates[playerIndex][actionIndex] = isPressed;
  }
}

glm::vec2 InputManager::getCursorPosition(SDL_Renderer *renderer) const {
  float mouseX, mouseY;

  SDL_GetMouseState(&mouseX, &mouseY);

  float logicalX, logicalY;
  SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &logicalX, &logicalY);

  return glm::vec2(logicalX, logicalY);
}