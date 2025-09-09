#pragma once

#include <SDL3/SDL.h>

#include <functional>
#include <map>
#include <string>

#include "managers/InputManager.h"
#include "managers/ResourceManager.h"

enum class ButtonState {
  HOVERED,
  NONE
};

enum class MainMenuAction {
  STARTGAME,
  QUIT,
  NONE
};

struct MainMenuButton {
  std::string text;

  SDL_FRect dimensions;
  ButtonState currentState;

  std::function<void()> pressCallback;
};

class MainMenu {
 public:
  MainMenu();
  ~MainMenu();

  void update(const InputManager &inputManager, SDL_Renderer *renderer);
  void render(SDL_Renderer *renderer);

  void startButtonCallback();

  void startGame();
  void quitGame();

  MainMenuAction getMainMenuAction() const { return actionToken; }

 private:
  std::map<int, MainMenuButton> buttons;

  MainMenuAction actionToken = MainMenuAction::NONE;

  SDL_Texture *renderText(SDL_Renderer *renderer, const std::string &text, SDL_Color color, int fontSize);
};