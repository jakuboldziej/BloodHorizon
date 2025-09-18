#pragma once

class GameConfig {
 public:
  // Logical rendering dimensions
  static constexpr int LOGICAL_WIDTH = 640;
  static constexpr int LOGICAL_HEIGHT = 360;

  // Default window scaling
  static constexpr int DEFAULT_WINDOW_SCALE = 2;
  static constexpr int DEFAULT_WINDOW_WIDTH = LOGICAL_WIDTH * DEFAULT_WINDOW_SCALE;
  static constexpr int DEFAULT_WINDOW_HEIGHT = LOGICAL_HEIGHT * DEFAULT_WINDOW_SCALE;

  // Game constants
  static constexpr float PLAYER1_X_RATIO = 0.2f;
  static constexpr float PLAYER2_X_RATIO = 0.8f;
  static constexpr float PLAYER_Y_RATIO = 0.9f;

 private:
  GameConfig() = delete;
};
