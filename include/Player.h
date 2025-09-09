#pragma once

#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Animation.h"
#include "utils/SDLDeleter.h"

class Player {
 public:
  Player(bool primaryPlayer);
  ~Player();

  void update();
  void render(SDL_Renderer *renderer);

 private:
  shared_texture idleTexture;
  shared_texture runTexture;
  shared_texture takingPunchTexture;
  SDL_FRect hitbox;

  glm::vec2 position, velocity;

  float direction;

  std::vector<Animation> animations;
  int currentAnimation;
  int spriteFrame;

  bool primaryPlayer;
};