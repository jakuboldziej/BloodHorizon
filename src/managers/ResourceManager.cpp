#include "managers/ResourceManager.h"

#include <SDL3_image/SDL_image.h>

bool ResourceManager::initialize(SDL_Renderer *renderer) {
  if (initialized) {
    return true;
  }

  this->renderer = renderer;

  // Initialize font manager
  fontManager.initialize();
  fontManager.loadFont("resources/fonts/vgasyse.ttf", 24);
  fontManager.loadFont("resources/fonts/vgasyse.ttf", 32);

  // Initialize animations
  animations[AnimationType::PLAYER1_IDLE] = Animation(8, 1.6f);
  animations[AnimationType::PLAYER1_RUN] = Animation(6, 0.75f);
  animations[AnimationType::PLAYER1_TAKING_PUNCH] = Animation(5, 0.75f);

  animations[AnimationType::PLAYER2_IDLE] = Animation(8, 1.6f);
  animations[AnimationType::PLAYER2_RUN] = Animation(6, 0.75f);
  animations[AnimationType::PLAYER2_TAKING_PUNCH] = Animation(5, 0.75f);

  // Pre-load common textures
  getTexture("resources/textures/player1/idle.png");
  getTexture("resources/textures/player1/run.png");
  getTexture("resources/textures/player1/taking-punch.png");

  initialized = true;
  return true;
}

void ResourceManager::cleanup() {
  if (!initialized) {
    return;
  }

  textureCache.clear();

  animations.clear();

  fontManager.cleanup();

  renderer = nullptr;
  initialized = false;
}

shared_texture ResourceManager::getTexture(const std::string &path) {
  if (!initialized || !renderer) {
    std::cerr << "ResourceManager not initialized!" << std::endl;
    return nullptr;
  }

  auto it = textureCache.find(path);
  if (it != textureCache.end()) {
    if (auto shared = it->second.lock()) {
      return shared;
    } else {
      textureCache.erase(it);
    }
  }

  auto texture = loadTexture(path);
  if (texture) {
    textureCache[path] = texture;
  }

  return texture;
}

shared_texture ResourceManager::loadTexture(const std::string &filePath) {
  SDL_Texture *tex = IMG_LoadTexture(renderer, filePath.c_str());
  if (!tex) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             std::format("Error loading texture: {} - {}", filePath, SDL_GetError()).c_str(), nullptr);
    return nullptr;
  }

  SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

  return std::shared_ptr<SDL_Texture>(tex, SDLDeleter{});
}

const Animation &ResourceManager::getAnimation(AnimationType type) const {
  auto it = animations.find(type);
  if (it != animations.end()) {
    return it->second;
  }

  static Animation defaultAnim(1, 1.0f);
  return defaultAnim;
}

std::vector<Animation> ResourceManager::getPlayerAnimations(bool isPrimaryPlayer) const {
  std::vector<Animation> playerAnims;
  playerAnims.reserve(3);

  if (isPrimaryPlayer) {
    playerAnims.push_back(getAnimation(AnimationType::PLAYER1_IDLE));
    playerAnims.push_back(getAnimation(AnimationType::PLAYER1_RUN));
    playerAnims.push_back(getAnimation(AnimationType::PLAYER1_TAKING_PUNCH));
  } else {
    playerAnims.push_back(getAnimation(AnimationType::PLAYER2_IDLE));
    playerAnims.push_back(getAnimation(AnimationType::PLAYER2_RUN));
    playerAnims.push_back(getAnimation(AnimationType::PLAYER2_TAKING_PUNCH));
  }

  return playerAnims;
}
