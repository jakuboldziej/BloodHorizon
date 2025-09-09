#pragma once

class GameConfig {
 public:
  // Logical rendering dimensions
  static constexpr int LOGICAL_WIDTH = 1280;
  static constexpr int LOGICAL_HEIGHT = 720;

  // Game constants
  static constexpr float PLAYER_SIZE = 64.0f;
  static constexpr float PLAYER1_X_RATIO = 0.2f;
  static constexpr float PLAYER2_X_RATIO = 0.8f;
  static constexpr float PLAYER_Y_RATIO = 0.9f;

 private:
  GameConfig() = delete;
};
