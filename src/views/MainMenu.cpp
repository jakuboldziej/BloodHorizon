#include "views/MainMenu.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <glm/glm.hpp>

#include "GameConfig.h"

MainMenu::MainMenu() {
  float buttonWidth = 150.0f;
  float buttonHeight = 45.0f;
  float buttonX = (GameConfig::LOGICAL_WIDTH - buttonWidth) / 2.0f;
  float buttonY = (GameConfig::LOGICAL_HEIGHT - buttonHeight) / 2.0f;

  buttons[0] = {"START", {buttonX, buttonY, buttonWidth, buttonHeight}, ButtonState::NONE, [this]() { this->startButtonCallback(); }};
}

MainMenu::~MainMenu() {
}

bool pointInRect(const SDL_FRect &rect, const glm::vec2 point) {
  bool xOverlap = point.x >= rect.x && point.x < rect.x + rect.w;
  bool yOverlap = point.y >= rect.y && point.y < rect.y + rect.h;

  return xOverlap && yOverlap;
}

void MainMenu::update(const InputManager &inputManager, SDL_Renderer *renderer) {
  for (auto &[id, button] : buttons) {
    glm::vec2 cursorPosition = inputManager.getCursorPosition(renderer);

    if (pointInRect(button.dimensions, cursorPosition)) {
      button.currentState = ButtonState::HOVERED;

      if (inputManager.primary)
        button.pressCallback();
    } else {
      button.currentState = ButtonState::NONE;
    }
  }
}

void MainMenu::render(SDL_Renderer *renderer) {
  // TODO: render background texture

  for (auto &[id, button] : buttons) {
    if (button.currentState == ButtonState::HOVERED) {
      SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    } else {
      SDL_SetRenderDrawColor(renderer, 60, 60, 100, 255);
    }

    SDL_RenderFillRect(renderer, &button.dimensions);

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderRect(renderer, &button.dimensions);

    // Render button text
    SDL_Color textColor = {255, 255, 255, 255};  // White text
    unique_texture textTexture(renderText(renderer, button.text, textColor, 24));

    if (textTexture != nullptr) {
      float textWidth, textHeight;
      SDL_GetTextureSize(textTexture.get(), &textWidth, &textHeight);

      // Center the text on the button
      SDL_FRect textRect = {
          button.dimensions.x + (button.dimensions.w - textWidth) / 2.0f,
          button.dimensions.y + (button.dimensions.h - textHeight) / 2.0f,
          textWidth,
          textHeight};

      SDL_RenderTexture(renderer, textTexture.get(), nullptr, &textRect);
    }
  }
}

void MainMenu::startButtonCallback() {
  startGame();
}

void MainMenu::startGame() {
  actionToken = MainMenuAction::STARTGAME;
}

void MainMenu::quitGame() {
  actionToken = MainMenuAction::QUIT;
}

SDL_Texture *MainMenu::renderText(SDL_Renderer *renderer, const std::string &text, SDL_Color color, int fontSize) {
  ResourceManager &resources = ResourceManager::getInstance();
  TTF_Font *font = resources.getFontManager().getFont(fontSize);
  if (font == nullptr) {
    std::cerr << "No font available for size " << fontSize << '\n';
    return nullptr;
  }

  unique_surface textSurface(TTF_RenderText_Solid(font, text.c_str(), text.length(), color));
  if (textSurface == nullptr) {
    std::cerr << "TTF_RenderText_Solid Error: " << SDL_GetError() << '\n';
    return nullptr;
  }

  SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface.get());

  return textTexture;
}