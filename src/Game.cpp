#include "Game.h"

#include <iostream>

#include "GameConfig.h"
#include "managers/DebugManager.h"

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

  DebugManager::getInstance().initialize(renderer.get());

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
  const int TARGET_FPS = 60;
  const double TARGET_FRAME_TIME = 1000.0 / TARGET_FPS;  // milliseconds

  uint64_t lastFrameTime = SDL_GetPerformanceCounter();

  while (running) {
    uint64_t currentTime = SDL_GetPerformanceCounter();
    double deltaTimeMs = (double)(currentTime - lastFrameTime) / SDL_GetPerformanceFrequency() * 1000.0;
    lastFrameTime = currentTime;

    inputManager.initProcessSession();
    SDL_Event event{0};
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          running = false;
          break;
        case SDL_EVENT_KEY_UP:
          if (event.key.scancode == SDL_SCANCODE_F1) {
            debugMode = !debugMode;
            DebugManager::getInstance().setDebugMode(debugMode);
          } else if (event.key.scancode == SDL_SCANCODE_F11) {
            fullscreen = !fullscreen;
            SDL_SetWindowFullscreen(window.get(), fullscreen);
          }
          break;
      }
      inputManager.processEvent(event);
    }

    update(deltaTimeMs / 1000.0f);  // Convert to seconds for compatibility

    render();

    // Frame rate limiting
    double frameTimeMs = (double)(SDL_GetPerformanceCounter() - currentTime) / SDL_GetPerformanceFrequency() * 1000.0;
    if (frameTimeMs < TARGET_FRAME_TIME) {
      SDL_Delay((Uint32)(TARGET_FRAME_TIME - frameTimeMs));
    }
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
      gameLoopView->update(inputManager, deltaTime);
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

  if (debugMode) {
    DebugManager::getInstance().clear();
    renderDebug();
    DebugManager::getInstance().render(renderer.get());
  }

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
  DebugManager &debug = DebugManager::getInstance();

  debug.debugGameState(static_cast<int>(currentGameState));

  auto pos = inputManager.getCursorPosition(renderer.get());
  debug.debugCursorPosition(pos.x, pos.y);

  debug.debugInputManager(inputManager);

  if (currentGameState == GameState::GAMELOOP && gameLoopView) {
    const Player *player1 = gameLoopView->getPlayer1();
    const Player *player2 = gameLoopView->getPlayer2();

    if (player1) {
      debug.debugPlayer(player1, "Player1");
    }

    if (player2) {
      debug.debugPlayer(player2, "Player2");
    }
  }
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