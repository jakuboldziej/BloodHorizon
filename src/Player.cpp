#include "Player.h"

#include <assert.h>

#include <iostream>

#include "GameConfig.h"
#include "managers/ResourceManager.h"

Player::Player(bool primaryPlayer)
    : primaryPlayer(primaryPlayer), spriteFrame(1), currentAnimation(0) {
  ResourceManager &resources = ResourceManager::getInstance();
  hitbox = SDL_FRect{};

  moveSpeed = 300.0f;  // pixels per second
  jumpPower = 500.0f;
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

    // if (primaryPlayer) {
    // float time = animations[currentAnimation].getTime();
    // float length = animations[currentAnimation].getLength();
    // if (primaryPlayer) {
    //   std::cout << "primaryPlayer: " << primaryPlayer << " Time: " << time << " Length: " << length << " Frame: " << frame << " srcX: " << srcX << std::endl;
    // }

  } else {
    srcX = (spriteFrame - 1) * frameWidth;
  }

  SDL_FRect src{
      .x = srcX,
      .y = 0,
      .w = frameWidth,
      .h = textureHeight};

  float scale = 2.0f;
  SDL_FRect dst{
      .x = position.x,
      .y = position.y - (textureHeight * scale),
      .w = frameWidth * scale,
      .h = textureHeight * scale};

  SDL_FlipMode flipMode = direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  SDL_RenderTextureRotated(renderer, currentTexture.get(), &src, &dst, 0, nullptr, flipMode);

  // if (debugMode) {
  //   SDL_FRect rectA{
  //       .x = position.x + hitbox.x,
  //       .y = position.y + hitbox.y,
  //       .w = hitbox.w,
  //       .h = hitbox.h};
  //   SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  //   SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
  //   SDL_RenderFillRect(renderer, &rectA);
  //   SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  // }
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