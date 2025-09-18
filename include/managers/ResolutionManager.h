#pragma once

#include <SDL3/SDL.h>

#include <utility>
#include <vector>

struct Resolution {
  int width;
  int height;

  Resolution(int w, int h) : width(w), height(h) {}

  bool operator==(const Resolution &other) const {
    return width == other.width && height == other.height;
  }
};

class ResolutionManager {
 public:
  static ResolutionManager &getInstance();

  void initialize(int logicalWidth, int logicalHeight, int defaultWindowWidth, int defaultWindowHeight);

  const std::vector<Resolution> &getAvailableResolutions() const;

  void setWindowResolution(SDL_Window *window, const Resolution &resolution);

  Resolution getCurrentWindowResolution() const;

  Resolution getLogicalResolution() const;

  float getScalingFactor() const;

  bool isPixelPerfect(const Resolution &resolution) const;

  Resolution getClosestPixelPerfectResolution(int width, int height) const;

  void toggleFullscreen(SDL_Window *window);
  void setFullscreen(SDL_Window *window, bool fullscreen);
  bool isFullscreen() const;
  Resolution getDesktopResolution() const;

 private:
  ResolutionManager() = default;

  Resolution logicalResolution{640, 360};
  Resolution currentWindowResolution{1280, 720};
  Resolution windowedResolution{1280, 720};
  std::vector<Resolution> availableResolutions;
  bool isCurrentlyFullscreen = false;

  void generateAvailableResolutions();
};
