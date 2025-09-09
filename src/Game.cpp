#include "Game.h"

#include <iostream>

#include "GameConfig.h"

Game::Game() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL", nullptr);
  }

  window = unique_window(SDL_CreateWindow("Blood Horizon", 1600, 900, 0));
  if (!window) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", nullptr);
    cleanup();
  }

  renderer = unique_renderer(SDL_CreateRenderer(window.get(), nullptr));
  if (!renderer) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating renderer", nullptr);
    cleanup();
  }

  ResourceManager &resources = ResourceManager::getInstance();
  if (!resources.initialize(renderer.get())) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing ResourceManager", nullptr);
    cleanup();
  }

  SDL_SetRenderVSync(renderer.get(), 1);

  SDL_SetRenderLogicalPresentation(renderer.get(), GameConfig::LOGICAL_WIDTH, GameConfig::LOGICAL_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

  switch (currentGameState) {
    case GameState::MAINMENU: {
      mainMenuView = std::make_unique<MainMenu>();
      break;
    }
    case GameState::GAMELOOP: {
      gameLoopView = std::make_unique<GameLoop>();
      break;
    }
    default: {
      mainMenuView = std::make_unique<MainMenu>();
      break;
    }
  }
}

Game::~Game() {
  cleanup();
}

void Game::run() {
  uint64_t prevTime = SDL_GetTicks();

  while (running) {
    uint64_t nowTime = SDL_GetTicks();
    float deltaTime = (nowTime - prevTime) / 1000.0f;

    // events

    inputManager.initProcessSession();
    SDL_Event event{0};
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT: {
          running = false;
          break;
        }
        case SDL_EVENT_KEY_UP: {
          if (event.key.scancode == SDL_SCANCODE_DELETE) {
            debugMode = !debugMode;
          } else if (event.key.scancode == SDL_SCANCODE_F11) {
            fullscreen = !fullscreen;
            SDL_SetWindowFullscreen(window.get(), fullscreen);
          }
        }
      }

      inputManager.processEvent(event);
    }

    update(deltaTime);

    render();

    prevTime = nowTime;
  }

  ResourceManager::getInstance().cleanup();
  cleanup();
}

void Game::update(float deltaTime) {
  switch (currentGameState) {
    case GameState::MAINMENU: {
      mainMenuView->update(inputManager, renderer.get());

      switch (mainMenuView->getMainMenuAction()) {
        case MainMenuAction::QUIT: {
          cleanup();
          break;
        }
        case MainMenuAction::STARTGAME: {
          changeGameState(GameState::GAMELOOP);
          break;
        }
        default:
          break;
      }
      break;
    }
    case GameState::GAMELOOP: {
      // Handle game loop updates here
      gameLoopView->update();
      break;
    }
    default:
      break;
  }
}

void Game::render() {
  SDL_SetRenderDrawColor(renderer.get(), 20, 10, 30, 255);
  SDL_RenderClear(renderer.get());

  renderUI();

  if (debugMode)
    renderDebug();

  // swap buffers and present
  SDL_RenderPresent(renderer.get());
}

void Game::renderUI() {
  switch (currentGameState) {
    case GameState::MAINMENU: {
      mainMenuView->render(renderer.get());
      break;
    }
    case GameState::GAMELOOP: {
      gameLoopView->render(renderer.get());
      break;
    }
    default:
      break;
  }
}

void Game::renderDebug() {
  // if (currentGameState != GameState::GAMELOOP)
  // return;

  SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
  SDL_RenderDebugText(renderer.get(), 5, 5, std::to_string(static_cast<int>(currentGameState)).c_str());

  auto pos = inputManager.getCursorPosition(renderer.get());
  std::string posStr = "Cursor: (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")";
  SDL_RenderDebugText(renderer.get(), 5, 15, posStr.c_str());
}

void Game::cleanup() {
  renderer.reset();
  window.reset();
  SDL_Quit();
}

void Game::changeGameState(GameState newGameState) {
  if (currentGameState == newGameState)
    return;

  switch (newGameState) {
    case GameState::GAMELOOP: {
      // TODO: unload rest views

      gameLoopView = std::make_unique<GameLoop>();
      currentGameState = GameState::GAMELOOP;
      break;
    }
    case GameState::MAINMENU: {
      // TODO: unload rest views

      mainMenuView = std::make_unique<MainMenu>();
      currentGameState = GameState::MAINMENU;
      break;
    }
    default:
      break;
  }
}