#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Animation.h"
#include "managers/FontManager.h"
#include "utils/SDLDeleter.h"

enum class AnimationType {
  PLAYER1_IDLE = 0,
  PLAYER1_RUN = 1,
  PLAYER1_TAKING_PUNCH = 2,
  PLAYER2_IDLE = 3,
  PLAYER2_RUN = 4,
  PLAYER2_TAKING_PUNCH = 5
};

class ResourceManager {
 public:
  // Singleton access
  static ResourceManager &getInstance() {
    static ResourceManager instance;
    return instance;
  }

  bool initialize(SDL_Renderer *renderer);

  void cleanup();

  shared_texture getTexture(const std::string &path);

  const Animation &getAnimation(AnimationType type) const;
  std::vector<Animation> getPlayerAnimations(bool isPrimaryPlayer) const;

  FontManager &getFontManager() { return fontManager; }

  ResourceManager(const ResourceManager &) = delete;
  ResourceManager &operator=(const ResourceManager &) = delete;

 private:
  ResourceManager() = default;
  ~ResourceManager() = default;

  shared_texture loadTexture(const std::string &filePath);

  SDL_Renderer *renderer = nullptr;
  std::unordered_map<std::string, std::weak_ptr<SDL_Texture>> textureCache;
  std::unordered_map<AnimationType, Animation> animations;
  FontManager fontManager;
  bool initialized = false;
};