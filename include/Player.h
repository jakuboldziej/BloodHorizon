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

  void update(float deltaTime);
  void render(SDL_Renderer *renderer);

  void setAnimation(int animationIndex);
  int getCurrentAnimation() const { return currentAnimation; }

  void playIdleAnimation() { setAnimation(0); }
  void playRunAnimation() { setAnimation(1); }
  void playTakingPunchAnimation() { setAnimation(2); }

  bool isPlayerGrounded() const { return isGrounded; }

  void move(float direction);
  void jump();
  void punch();
  void stopMoving();

  glm::vec2 getPosition() const { return position; }
  void setPosition(const glm::vec2 &newPosition) {
    position = newPosition;
    velocity *= 0.8f;
  }
  bool isMoving() const { return velocity.x != 0; }

  SDL_FRect getWorldHitbox() const;
  SDL_FRect getAttackBox() const;
  void applyKnockback(const glm::vec2 &knockbackVelocity);

  SDL_FRect getLocalHitbox() const { return hitbox; }

 private:
  shared_texture idleTexture;
  shared_texture runTexture;
  shared_texture takingPunchTexture;
  SDL_FRect hitbox;

  glm::vec2 position, velocity;

  float direction;
  float moveSpeed;
  float jumpPower;
  bool isGrounded;
  bool isActivelyMoving;

  std::vector<Animation> animations;
  int currentAnimation;
  int spriteFrame;

  bool primaryPlayer;
};