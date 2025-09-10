#include "managers/FontManager.h"

#include <iostream>
#include <vector>

FontManager::FontManager() {
}

FontManager::~FontManager() {
  cleanup();
}

bool FontManager::initialize() {
  if (TTF_Init() == -1) {
    std::cerr << "TTF_Init Error: " << SDL_GetError() << '\n';
    return false;
  }
  return true;
}

void FontManager::cleanup() {
  fonts.clear();
  TTF_Quit();
}

TTF_Font *FontManager::getFont(int size) const {
  auto it = fonts.find(size);
  if (it != fonts.end()) {
    return it->second.get();
  }
  return nullptr;
}

bool FontManager::loadFont(const std::string &fontPath, int size) {
  if (fonts.find(size) != fonts.end()) {
    return true;
  }

  TTF_Font *font = nullptr;

  font = TTF_OpenFont(fontPath.c_str(), size);

  if (font == nullptr) {
    std::cerr << "TTF_OpenFont Error for " << fontPath << ": " << SDL_GetError() << '\n';

    std::vector<std::string> fallbackFonts = {
        "C:/Windows/Fonts/consola.ttf",  // Console font, similar to VGA
        "C:/Windows/Fonts/cour.ttf",     // Courier New
        "C:/Windows/Fonts/arial.ttf",    // Arial
        "C:/Windows/Fonts/calibri.ttf",  // Calibri
    };

    for (const auto &fallback : fallbackFonts) {
      font = TTF_OpenFont(fallback.c_str(), size);
      if (font != nullptr) {
        std::cout << "Using fallback font: " << fallback << " (size " << size << ")" << '\n';
        break;
      }
    }
  }

  if (font == nullptr) {
    std::cerr << "Failed to load any font for size " << size << '\n';
    std::cerr << "Note: .fon files are not supported. Please use .ttf or .otf fonts." << '\n';
    return false;
  }

  fonts[size] = shared_font(font, SDLDeleter{});
  std::cout << "Successfully loaded font at size " << size << '\n';
  return true;
}
