#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#include <map>
#include <string>

class FontManager {
 public:
  FontManager();
  ~FontManager();

  bool initialize();
  void cleanup();

  TTF_Font *getFont(int size) const;
  bool loadFont(const std::string &fontPath, int size);

 private:
  std::map<int, TTF_Font *> fonts;
  std::string defaultFontPath;
};