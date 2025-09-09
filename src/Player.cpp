#include "Player.h"

#include <assert.h>

#include <iostream>

#include "GameConfig.h"
#include "managers/ResourceManager.h"

Player::Player(bool primaryPlayer)
    : primaryPlayer(primaryPlayer), spriteFrame(1), currentAnimation(0) {
  ResourceManager &resources = ResourceManager::getInstance();
  hitbox = SDL_FRect{};

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
}

Player::~Player() {
}

void Player::update() {
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
  if (currentAnimation != -1 && animations.size() > currentAnimation) {
    frameWidth = textureWidth / 8;  // Assuming max 8 frames for now
  }

  float srcX = currentAnimation != -1 && animations.size() > currentAnimation
                   ? animations[currentAnimation].currentFrame() * frameWidth
                   : (spriteFrame - 1) * frameWidth;

  SDL_FRect src{
      .x = srcX,
      .y = 0,
      .w = frameWidth,
      .h = textureHeight};

  SDL_FRect dst{
      .x = position.x,
      .y = position.y,
      .w = frameWidth,
      .h = textureHeight};

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