#pragma once

#include <SDL3/SDL.h>

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Player;

enum class CollisionType {
  PLAYER_BOUNDARY,
  PLAYER_VS_PLAYER,
  ATTACK_HIT,
  PROJECTILE_HIT,
  STAGE_BOUNDARY
};

enum class CollisionLayer {
  PLAYER = 1 << 0,
  ATTACK = 1 << 1,
  PROJECTILE = 1 << 2,
  STAGE = 1 << 3,
  PICKUP = 1 << 4
};

struct CollisionInfo {
  CollisionType type;
  glm::vec2 contactPoint;
  glm::vec2 normal;
  float penetration;
  void *entityA;
  void *entityB;
};

class ICollidable {
 public:
  virtual ~ICollidable() = default;
  virtual SDL_FRect getCollisionBox() const = 0;
  virtual SDL_FRect getAttackBox() const = 0;
  virtual CollisionLayer getCollisionLayer() const = 0;
  virtual bool isCollisionEnabled() const = 0;
  virtual void onCollision(const CollisionInfo &info) = 0;
  virtual glm::vec2 getPosition() const = 0;
  virtual glm::vec2 getVelocity() const = 0;
};

class CollisionManager {
 public:
  static CollisionManager &getInstance();

  void update(float deltaTime);
  void checkAllCollisions();

  void checkPlayerCollisions(Player *player1, Player *player2);
  void checkPlayerBoundaryCollisions(Player *player);
  void checkPlayerAttackCollisions(Player *attacker, Player *defender);

  void registerCollidable(std::shared_ptr<ICollidable> collidable);
  void unregisterCollidable(std::shared_ptr<ICollidable> collidable);
  void clearAll();

  void setWorldBounds(const SDL_FRect &bounds);
  SDL_FRect getWorldBounds() const { return worldBounds; }

  bool checkCollision(const SDL_FRect &a, const SDL_FRect &b) const;
  bool checkPointInRect(const glm::vec2 &point, const SDL_FRect &rect) const;
  std::vector<std::shared_ptr<ICollidable>> getCollidablesInArea(const SDL_FRect &area) const;

  void resolvePlayerCollision(Player *player1, Player *player2);
  void resolvePlayerBoundaryCollision(Player *player, const SDL_FRect &boundary);
  void resolveAttackHit(Player *attacker, Player *defender);

  std::vector<CollisionInfo> getLastFrameCollisions() const { return lastFrameCollisions; }
  void setDebugVisualization(bool enabled) { debugVisualization = enabled; }
  bool isDebugVisualizationEnabled() const { return debugVisualization; }

  void setOnPlayerHitCallback(std::function<void(Player *, Player *)> callback);
  void setOnBoundaryHitCallback(std::function<void(Player *)> callback);

 private:
  CollisionManager() = default;
  ~CollisionManager() = default;
  CollisionManager(const CollisionManager &) = delete;
  CollisionManager &operator=(const CollisionManager &) = delete;

  bool checkAABBCollision(const SDL_FRect &a, const SDL_FRect &b, CollisionInfo *info = nullptr) const;
  bool checkCircleCollision(const glm::vec2 &centerA, float radiusA,
                            const glm::vec2 &centerB, float radiusB) const;

  glm::vec2 calculateSeparationVector(const SDL_FRect &a, const SDL_FRect &b) const;
  float calculatePenetrationDepth(const SDL_FRect &a, const SDL_FRect &b) const;

  std::vector<std::shared_ptr<ICollidable>> collidables;
  std::vector<CollisionInfo> lastFrameCollisions;
  SDL_FRect worldBounds = {0, 0, 640, 360};
  bool debugVisualization = false;

  std::function<void(Player *, Player *)> onPlayerHitCallback;
  std::function<void(Player *)> onBoundaryHitCallback;

  static constexpr float COLLISION_EPSILON = 0.001f;
  static constexpr int MAX_COLLISION_ITERATIONS = 4;
};