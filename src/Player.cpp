#include "Player.h"

#include <assert.h>

#include <iostream>

#include "GameConfig.h"
#include "managers/DebugManager.h"
#include "managers/ResourceManager.h"

Player::Player(bool primaryPlayer)
    : primaryPlayer(primaryPlayer), spriteFrame(1), currentAnimation(0) {
  ResourceManager &resources = ResourceManager::getInstance();

  moveSpeed = 200.0f;  // pixels per second
  jumpPower = 300.0f;
  isGrounded = true;
  isActivelyMoving = false;
  velocity = glm::vec2(0, 0);

  if (primaryPlayer) {
    position = glm::vec2(GameConfig::LOGICAL_WIDTH * GameConfig::PLAYER1_X_RATIO, GameConfig::LOGICAL_HEIGHT * GameConfig::PLAYER_Y_RATIO);

    idleTexture = resources.getTexture("resources/textures/player1/idle.png");
    runTexture = resources.getTexture("resources/textures/player1/run.png");
    takingPunchTexture = resources.getTexture("resources/textures/player1/taking-punch.png");

    direction = 1;
  } else {
    position = glm::vec2(GameConfig::LOGICAL_WIDTH * GameConfig::PLAYER2_X_RATIO, GameConfig::LOGICAL_HEIGHT * GameConfig::PLAYER_Y_RATIO);

    idleTexture = resources.getTexture("resources/textures/player1/idle.png");
    runTexture = resources.getTexture("resources/textures/player1/run.png");
    takingPunchTexture = resources.getTexture("resources/textures/player1/taking-punch.png");

    direction = -1;
  }

  animations.resize(3);
  animations = resources.getPlayerAnimations(primaryPlayer);

  if (idleTexture) {
    float textureWidth, textureHeight;
    SDL_GetTextureSize(idleTexture.get(), &textureWidth, &textureHeight);

    float frameWidth = textureWidth;
    if (!animations.empty()) {
      frameWidth = textureWidth / animations[0].getFrameCount();
    }

    hitbox = SDL_FRect{
        .x = frameWidth * 0.2f,
        .y = textureHeight * 0.1f,
        .w = frameWidth * 0.6f,
        .h = textureHeight * 0.8f};
  } else {
    hitbox = SDL_FRect{.x = 10, .y = 10, .w = 32, .h = 48};
  }
}

Player::~Player() {
}

void Player::update(float deltaTime) {
  if (!isActivelyMoving) {
    velocity.x *= 0.85f;

    if (abs(velocity.x) < 10.0f) {
      velocity.x = 0;
    }
  }

  position.x += velocity.x * deltaTime;
  position.y += velocity.y * deltaTime;

  float maxSpeed = 500.0f;
  if (abs(velocity.x) > maxSpeed) {
    velocity.x = (velocity.x > 0) ? maxSpeed : -maxSpeed;
  }
  if (abs(velocity.y) > maxSpeed) {
    velocity.y = (velocity.y > 0) ? maxSpeed : -maxSpeed;
  }

  if (isActivelyMoving) {
    if (currentAnimation != 1) {
      playRunAnimation();
    }
  } else if (abs(velocity.x) < 20.0f) {
    if (currentAnimation == 1) {
      playIdleAnimation();
    }
  }

  if (currentAnimation >= 0 && currentAnimation < animations.size()) {
    animations[currentAnimation].step(deltaTime);

    if (animations[currentAnimation].isDone() && currentAnimation == 2) {
      setAnimation(0);
    }
  }
}

void Player::setAnimation(int animationIndex) {
  if (animationIndex >= 0 && animationIndex < animations.size() && animationIndex != currentAnimation) {
    currentAnimation = animationIndex;
    animations[currentAnimation].reset();
  }
}

void Player::render(SDL_Renderer *renderer) {
  shared_texture currentTexture;

  if (currentAnimation == 0) {  // Idle
    currentTexture = idleTexture;
  } else if (currentAnimation == 1) {  // Run
    currentTexture = runTexture;
  } else if (currentAnimation == 2) {  // Taking punch
    currentTexture = takingPunchTexture;
  } else {
    currentTexture = idleTexture;  // Default to idle
  }

  if (!currentTexture) {
    return;
  }

  float textureWidth, textureHeight;
  SDL_GetTextureSize(currentTexture.get(), &textureWidth, &textureHeight);

  float frameWidth = textureWidth;
  if (currentAnimation >= 0 && currentAnimation < animations.size()) {
    frameWidth = textureWidth / animations[currentAnimation].getFrameCount();
  }

  float srcX = 0;
  if (currentAnimation >= 0 && currentAnimation < animations.size()) {
    int frame = animations[currentAnimation].currentFrame();
    srcX = frame * frameWidth;
  } else {
    srcX = (spriteFrame - 1) * frameWidth;
  }

  SDL_FRect src{
      .x = srcX,
      .y = 0,
      .w = frameWidth,
      .h = textureHeight};

  SDL_FRect dst{
      .x = position.x,
      .y = position.y - (textureHeight),
      .w = frameWidth,
      .h = textureHeight};

  SDL_FlipMode flipMode = direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  SDL_RenderTextureRotated(renderer, currentTexture.get(), &src, &dst, 0, nullptr, flipMode);

  if (DebugManager::getInstance().isDebugMode()) {
    SDL_FRect rectA{
        .x = position.x + hitbox.x,
        .y = position.y - textureHeight + hitbox.y,
        .w = hitbox.w,
        .h = hitbox.h};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
    SDL_RenderFillRect(renderer, &rectA);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  }
}

void Player::move(float direction) {
  velocity.x = direction * moveSpeed;
  isActivelyMoving = true;

  this->direction = direction;
}

void Player::jump() {
}

void Player::punch() {
}

void Player::stopMoving() {
  isActivelyMoving = false;
}

SDL_FRect Player::getWorldHitbox() const {
  shared_texture currentTexture = idleTexture;
  if (currentAnimation == 1) {
    currentTexture = runTexture;
  } else if (currentAnimation == 2) {
    currentTexture = takingPunchTexture;
  }

  float textureHeight = 48.0f;
  if (currentTexture) {
    float textureWidth, tempHeight;
    SDL_GetTextureSize(currentTexture.get(), &textureWidth, &tempHeight);
    textureHeight = tempHeight;
  }

  return SDL_FRect{
      .x = position.x + hitbox.x,
      .y = position.y - textureHeight + hitbox.y,
      .w = hitbox.w,
      .h = hitbox.h};
}

SDL_FRect Player::getAttackBox() const {
  if (currentAnimation != 2) {
    return SDL_FRect{0, 0, 0, 0};
  }

  SDL_FRect worldHitbox = getWorldHitbox();

  float attackWidth = 30.0f;
  float attackHeight = worldHitbox.h * 0.6f;

  float attackX;
  if (direction > 0) {
    attackX = worldHitbox.x + worldHitbox.w;
  } else {
    attackX = worldHitbox.x - attackWidth;
  }

  return SDL_FRect{
      .x = attackX,
      .y = worldHitbox.y + worldHitbox.h * 0.2f,
      .w = attackWidth,
      .h = attackHeight};
}

void Player::applyKnockback(const glm::vec2 &knockbackVelocity) {
  velocity += knockbackVelocity;

  float maxKnockback = 400.0f;
  if (glm::length(velocity) > maxKnockback) {
    velocity = glm::normalize(velocity) * maxKnockback;
  }
}