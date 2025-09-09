#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "Animation.h"
#include "managers/FontManager.h"

class Resources {
 public:
  const int ANIM_PLAYER1_IDLE = 0;
  const int ANIM_PLAYER1_RUN = 1;
  const int ANIM_PLAYER1_TAKING_PUNCH = 2;
  std::vector<Animation> player1Anims;

  std::vector<SDL_Texture *> textures;
  SDL_Texture *texIdle1, *texRun1, *texTakingPunch1;

  FontManager fontManager;

  SDL_Texture *loadTexture(SDL_Renderer *renderer, const std::string &filePath) {
    SDL_Texture *tex = IMG_LoadTexture(renderer, filePath.c_str());
    if (!tex) {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", std::format("Error creating renderer : {} - {}", filePath, SDL_GetError()).c_str(), nullptr);
      return nullptr;
    }
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    textures.push_back(tex);
    return tex;
  }

  void load(SDL_Renderer *renderer) {
    // Fonts
    fontManager.initialize();

    fontManager.loadFont("resources/fonts/vgasyse.ttf", 24);
    fontManager.loadFont("resources/fonts/vgasyse.ttf", 32);

    // Player1
    player1Anims.resize(3);
    player1Anims[ANIM_PLAYER1_IDLE] = Animation(8, 1.6f);
    player1Anims[ANIM_PLAYER1_RUN] = Animation(6, 0.75f);
    player1Anims[ANIM_PLAYER1_TAKING_PUNCH] = Animation(5, 0.75f);

    textures.reserve(3);
    texIdle1 = loadTexture(renderer, "resources/textures/player1/idle.png");
    texRun1 = loadTexture(renderer, "resources/textures/player1/run.png");
    texTakingPunch1 = loadTexture(renderer, "resources/textures/player1/taking-punch.png");
  }

  void unload() {
    for (SDL_Texture *tex : textures) {
      SDL_DestroyTexture(tex);
    }
    fontManager.cleanup();
  }
};