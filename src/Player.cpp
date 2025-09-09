#include "Player.h"

#include <iostream>

#include "GameConfig.h"

Player::Player(bool primaryPlayer) : primaryPlayer(primaryPlayer) {
  hitbox = SDL_FRect{};
  if (primaryPlayer) {
    hitbox.x = GameConfig::LOGICAL_WIDTH * GameConfig::PLAYER1_X_RATIO;
    hitbox.y = GameConfig::LOGICAL_HEIGHT * GameConfig::PLAYER_Y_RATIO;
    hitbox.w = GameConfig::PLAYER_SIZE;
    hitbox.h = GameConfig::PLAYER_SIZE;
  } else {
    hitbox.x = GameConfig::LOGICAL_WIDTH * GameConfig::PLAYER2_X_RATIO - GameConfig::PLAYER_SIZE;
    hitbox.y = GameConfig::LOGICAL_HEIGHT * GameConfig::PLAYER_Y_RATIO;
    hitbox.w = GameConfig::PLAYER_SIZE;
    hitbox.h = GameConfig::PLAYER_SIZE;
  }
}

Player::~Player() {
}

void Player::update() {
}

void Player::render(SDL_Renderer *renderer) {
  if (primaryPlayer) {
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
  }

  SDL_RenderFillRect(renderer, &hitbox);
}