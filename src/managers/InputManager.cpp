#include "managers/InputManager.h"

void InputManager::initProcessSession() {
  primary = false;
}

void InputManager::processEvent(SDL_Event event) {
  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      primary = true;
  }
}

glm::vec2 InputManager::getCursorPosition(SDL_Renderer *renderer) const {
  float mouseX, mouseY;

  SDL_GetMouseState(&mouseX, &mouseY);

  float logicalX, logicalY;
  SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &logicalX, &logicalY);

  return glm::vec2(logicalX, logicalY);
}