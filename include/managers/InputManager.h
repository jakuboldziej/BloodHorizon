#pragma once

#include <SDL3/SDL.h>

#include "glm/glm.hpp"

class InputManager {
 public:
  bool primary = false;

  void initProcessSession();
  void processEvent(SDL_Event);

  glm::vec2 getCursorPosition(SDL_Renderer *renderer) const;
};