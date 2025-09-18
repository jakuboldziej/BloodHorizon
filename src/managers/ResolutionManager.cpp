#include "managers/ResolutionManager.h"

#include <algorithm>

ResolutionManager &ResolutionManager::getInstance() {
  static ResolutionManager instance;
  return instance;
}

void ResolutionManager::initialize(int logicalWidth, int logicalHeight, int defaultWindowWidth, int defaultWindowHeight) {
  logicalResolution = Resolution(logicalWidth, logicalHeight);
  currentWindowResolution = Resolution(defaultWindowWidth, defaultWindowHeight);
  generateAvailableResolutions();
}

const std::vector<Resolution> &ResolutionManager::getAvailableResolutions() const {
  return availableResolutions;
}

void ResolutionManager::setWindowResolution(SDL_Window *window, const Resolution &resolution) {
  if (window) {
    SDL_SetWindowSize(window, resolution.width, resolution.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    currentWindowResolution = resolution;

    if (!isCurrentlyFullscreen) {
      windowedResolution = resolution;
    }
  }
}

Resolution ResolutionManager::getCurrentWindowResolution() const {
  return currentWindowResolution;
}

Resolution ResolutionManager::getLogicalResolution() const {
  return logicalResolution;
}

float ResolutionManager::getScalingFactor() const {
  float scaleX = static_cast<float>(currentWindowResolution.width) / logicalResolution.width;
  float scaleY = static_cast<float>(currentWindowResolution.height) / logicalResolution.height;
  return std::min(scaleX, scaleY);
}

bool ResolutionManager::isPixelPerfect(const Resolution &resolution) const {
  return (resolution.width % logicalResolution.width == 0) &&
         (resolution.height % logicalResolution.height == 0) &&
         (resolution.width / logicalResolution.width == resolution.height / logicalResolution.height);
}

Resolution ResolutionManager::getClosestPixelPerfectResolution(int width, int height) const {
  int scaleX = width / logicalResolution.width;
  int scaleY = height / logicalResolution.height;
  int scale = std::min(scaleX, scaleY);

  if (scale < 1)
    scale = 1;

  return Resolution(logicalResolution.width * scale, logicalResolution.height * scale);
}

void ResolutionManager::generateAvailableResolutions() {
  availableResolutions.clear();

  const std::vector<int> scales = {1, 2, 3, 4, 5, 6};

  for (int scale : scales) {
    int width = logicalResolution.width * scale;
    int height = logicalResolution.height * scale;

    if (width <= 3840 && height <= 2160) {
      availableResolutions.emplace_back(width, height);
    }
  }

  const std::vector<Resolution> commonResolutions = {
      {1280, 720},   // 720p
      {1366, 768},   // Common laptop resolution
      {1920, 1080},  // 1080p
      {2560, 1440},  // 1440p
      {3840, 2160}   // 4K
  };

  for (const auto &res : commonResolutions) {
    if (std::find(availableResolutions.begin(), availableResolutions.end(), res) == availableResolutions.end()) {
      availableResolutions.push_back(res);
    }
  }

  std::sort(availableResolutions.begin(), availableResolutions.end(),
            [](const Resolution &a, const Resolution &b) {
              return (a.width * a.height) < (b.width * b.height);
            });
}

void ResolutionManager::toggleFullscreen(SDL_Window *window) {
  setFullscreen(window, !isCurrentlyFullscreen);
}

void ResolutionManager::setFullscreen(SDL_Window *window, bool fullscreen) {
  if (!window)
    return;

  if (fullscreen && !isCurrentlyFullscreen) {
    windowedResolution = currentWindowResolution;

    Resolution desktopRes = getDesktopResolution();

    SDL_SetWindowFullscreen(window, true);
    currentWindowResolution = desktopRes;
    isCurrentlyFullscreen = true;

  } else if (!fullscreen && isCurrentlyFullscreen) {
    SDL_SetWindowFullscreen(window, false);
    setWindowResolution(window, windowedResolution);
    isCurrentlyFullscreen = false;
  }
}

bool ResolutionManager::isFullscreen() const {
  return isCurrentlyFullscreen;
}

Resolution ResolutionManager::getDesktopResolution() const {
  const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
  if (mode) {
    return Resolution(mode->w, mode->h);
  }
  return Resolution(1920, 1080);
}
