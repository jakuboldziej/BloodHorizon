#pragma once

#include <SDL3/SDL.h>

class Player {
 public:
  Player(bool primaryPlayer);
  ~Player();

  void update();
  void render(SDL_Renderer *renderer);

 private:
  SDL_Texture *texture;
  SDL_FRect hitbox;

  int currentAnimation;
  bool primaryPlayer;
};