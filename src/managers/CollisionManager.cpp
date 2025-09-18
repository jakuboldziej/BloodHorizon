#include "managers/CollisionManager.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "GameConfig.h"
#include "Player.h"

CollisionManager &CollisionManager::getInstance() {
  static CollisionManager instance;
  return instance;
}

void CollisionManager::update(float deltaTime) {
  lastFrameCollisions.clear();

  checkAllCollisions();
}

void CollisionManager::checkAllCollisions() {
  for (size_t i = 0; i < collidables.size(); ++i) {
    for (size_t j = i + 1; j < collidables.size(); ++j) {
      auto &objA = collidables[i];
      auto &objB = collidables[j];

      if (!objA->isCollisionEnabled() || !objB->isCollisionEnabled()) {
        continue;
      }

      int layerA = static_cast<int>(objA->getCollisionLayer());
      int layerB = static_cast<int>(objB->getCollisionLayer());

      bool shouldCollide = (layerA & layerB) != 0;

      if (shouldCollide) {
        CollisionInfo info;
        if (checkAABBCollision(objA->getCollisionBox(), objB->getCollisionBox(), &info)) {
          info.entityA = objA.get();
          info.entityB = objB.get();

          lastFrameCollisions.push_back(info);

          objA->onCollision(info);
          objB->onCollision(info);
        }
      }
    }
  }
}

void CollisionManager::checkPlayerCollisions(Player *player1, Player *player2) {
  if (!player1 || !player2)
    return;

  SDL_FRect box1 = player1->getWorldHitbox();
  SDL_FRect box2 = player2->getWorldHitbox();

  CollisionInfo info;
  if (checkAABBCollision(box1, box2, &info)) {
    info.type = CollisionType::PLAYER_VS_PLAYER;
    info.entityA = player1;
    info.entityB = player2;

    lastFrameCollisions.push_back(info);

    if (info.penetration > COLLISION_EPSILON) {
      resolvePlayerCollision(player1, player2);
    }
  }
}

void CollisionManager::checkPlayerBoundaryCollisions(Player *player) {
  if (!player)
    return;

  SDL_FRect playerBox = player->getWorldHitbox();

  bool collided = false;
  glm::vec2 playerPos = player->getPosition();

  if (playerBox.x < worldBounds.x) {
    CollisionInfo info;
    info.type = CollisionType::PLAYER_BOUNDARY;
    info.contactPoint = glm::vec2(worldBounds.x, playerPos.y);
    info.normal = glm::vec2(1, 0);
    info.penetration = worldBounds.x - playerBox.x;
    info.entityA = player;
    info.entityB = nullptr;

    lastFrameCollisions.push_back(info);
    resolvePlayerBoundaryCollision(player, worldBounds);
    collided = true;
  }

  if (playerBox.x + playerBox.w > worldBounds.x + worldBounds.w) {
    CollisionInfo info;
    info.type = CollisionType::PLAYER_BOUNDARY;
    info.contactPoint = glm::vec2(worldBounds.x + worldBounds.w, playerPos.y);
    info.normal = glm::vec2(-1, 0);
    info.penetration = (playerBox.x + playerBox.w) - (worldBounds.x + worldBounds.w);
    info.entityA = player;
    info.entityB = nullptr;

    lastFrameCollisions.push_back(info);
    resolvePlayerBoundaryCollision(player, worldBounds);
    collided = true;
  }

  if (playerBox.y < worldBounds.y) {
    CollisionInfo info;
    info.type = CollisionType::PLAYER_BOUNDARY;
    info.contactPoint = glm::vec2(playerPos.x, worldBounds.y);
    info.normal = glm::vec2(0, 1);
    info.penetration = worldBounds.y - playerBox.y;
    info.entityA = player;
    info.entityB = nullptr;

    lastFrameCollisions.push_back(info);
    resolvePlayerBoundaryCollision(player, worldBounds);
    collided = true;
  }

  if (playerBox.y + playerBox.h > worldBounds.y + worldBounds.h) {
    CollisionInfo info;
    info.type = CollisionType::PLAYER_BOUNDARY;
    info.contactPoint = glm::vec2(playerPos.x, worldBounds.y + worldBounds.h);
    info.normal = glm::vec2(0, -1);
    info.penetration = (playerBox.y + playerBox.h) - (worldBounds.y + worldBounds.h);
    info.entityA = player;
    info.entityB = nullptr;

    lastFrameCollisions.push_back(info);
    resolvePlayerBoundaryCollision(player, worldBounds);
    collided = true;
  }

  if (collided && onBoundaryHitCallback) {
    onBoundaryHitCallback(player);
  }
}

void CollisionManager::checkPlayerAttackCollisions(Player *attacker, Player *defender) {
  if (!attacker || !defender)
    return;

  if (attacker->getCurrentAnimation() != 2)
    return;

  SDL_FRect attackBox = attacker->getAttackBox();
  SDL_FRect defenderBox = defender->getWorldHitbox();

  CollisionInfo info;
  if (checkAABBCollision(attackBox, defenderBox, &info)) {
    info.type = CollisionType::ATTACK_HIT;
    info.entityA = attacker;
    info.entityB = defender;

    lastFrameCollisions.push_back(info);

    resolveAttackHit(attacker, defender);

    if (onPlayerHitCallback) {
      onPlayerHitCallback(attacker, defender);
    }
  }
}

void CollisionManager::registerCollidable(std::shared_ptr<ICollidable> collidable) {
  if (collidable) {
    collidables.push_back(collidable);
  }
}

void CollisionManager::unregisterCollidable(std::shared_ptr<ICollidable> collidable) {
  auto it = std::find(collidables.begin(), collidables.end(), collidable);
  if (it != collidables.end()) {
    collidables.erase(it);
  }
}

void CollisionManager::clearAll() {
  collidables.clear();
  lastFrameCollisions.clear();
}

void CollisionManager::setWorldBounds(const SDL_FRect &bounds) {
  worldBounds = bounds;
}

bool CollisionManager::checkCollision(const SDL_FRect &a, const SDL_FRect &b) const {
  return checkAABBCollision(a, b);
}

bool CollisionManager::checkPointInRect(const glm::vec2 &point, const SDL_FRect &rect) const {
  return point.x >= rect.x && point.x <= rect.x + rect.w &&
         point.y >= rect.y && point.y <= rect.y + rect.h;
}

std::vector<std::shared_ptr<ICollidable>> CollisionManager::getCollidablesInArea(const SDL_FRect &area) const {
  std::vector<std::shared_ptr<ICollidable>> result;

  for (const auto &collidable : collidables) {
    if (checkAABBCollision(area, collidable->getCollisionBox())) {
      result.push_back(collidable);
    }
  }

  return result;
}

void CollisionManager::resolvePlayerCollision(Player *player1, Player *player2) {
  if (!player1 || !player2)
    return;

  SDL_FRect box1 = player1->getWorldHitbox();
  SDL_FRect box2 = player2->getWorldHitbox();

  float overlapX = std::min(box1.x + box1.w, box2.x + box2.w) - std::max(box1.x, box2.x);
  float overlapY = std::min(box1.y + box1.h, box2.y + box2.h) - std::max(box1.y, box2.y);

  if (overlapX <= 0 || overlapY <= 0)
    return;

  glm::vec2 pos1 = player1->getPosition();
  glm::vec2 pos2 = player2->getPosition();

  // Calculate centers
  glm::vec2 center1(box1.x + box1.w * 0.5f, box1.y + box1.h * 0.5f);
  glm::vec2 center2(box2.x + box2.w * 0.5f, box2.y + box2.h * 0.5f);

  if (overlapX < overlapY) {
    float separation = overlapX * 0.51f;
    if (center1.x < center2.x) {
      player1->setPosition(glm::vec2(pos1.x - separation, pos1.y));
      player2->setPosition(glm::vec2(pos2.x + separation, pos2.y));
    } else {
      player1->setPosition(glm::vec2(pos1.x + separation, pos1.y));
      player2->setPosition(glm::vec2(pos2.x - separation, pos2.y));
    }
  } else {
    float separation = overlapY * 0.51f;
    if (center1.y < center2.y) {
      player1->setPosition(glm::vec2(pos1.x, pos1.y - separation));
      player2->setPosition(glm::vec2(pos2.x, pos2.y + separation));
    } else {
      player1->setPosition(glm::vec2(pos1.x, pos1.y + separation));
      player2->setPosition(glm::vec2(pos2.x, pos2.y - separation));
    }
  }
}

void CollisionManager::resolvePlayerBoundaryCollision(Player *player, const SDL_FRect &boundary) {
  if (!player)
    return;

  SDL_FRect playerBox = player->getWorldHitbox();
  glm::vec2 pos = player->getPosition();

  float newX = pos.x;
  float newY = pos.y;

  if (playerBox.x < boundary.x) {
    newX = pos.x + (boundary.x - playerBox.x);
  } else if (playerBox.x + playerBox.w > boundary.x + boundary.w) {
    newX = pos.x - ((playerBox.x + playerBox.w) - (boundary.x + boundary.w));
  }

  if (playerBox.y < boundary.y) {
    newY = pos.y + (boundary.y - playerBox.y);
  } else if (playerBox.y + playerBox.h > boundary.y + boundary.h) {
    newY = pos.y - ((playerBox.y + playerBox.h) - (boundary.y + boundary.h));
  }

  player->setPosition(glm::vec2(newX, newY));
}

void CollisionManager::resolveAttackHit(Player *attacker, Player *defender) {
  if (!attacker || !defender)
    return;

  defender->playTakingPunchAnimation();

  glm::vec2 attackerPos = attacker->getPosition();
  glm::vec2 defenderPos = defender->getPosition();

  glm::vec2 knockbackDir = defenderPos - attackerPos;
  float length = glm::length(knockbackDir);
  if (length > 0) {
    knockbackDir = glm::normalize(knockbackDir);
  } else {
    knockbackDir = glm::vec2(1, 0);
  }

  float knockbackForce = 150.0f;
  defender->applyKnockback(knockbackDir * knockbackForce);
}

void CollisionManager::setOnPlayerHitCallback(std::function<void(Player *, Player *)> callback) {
  onPlayerHitCallback = callback;
}

void CollisionManager::setOnBoundaryHitCallback(std::function<void(Player *)> callback) {
  onBoundaryHitCallback = callback;
}

bool CollisionManager::checkAABBCollision(const SDL_FRect &a, const SDL_FRect &b, CollisionInfo *info) const {
  bool collision = !(a.x + a.w <= b.x || b.x + b.w <= a.x ||
                     a.y + a.h <= b.y || b.y + b.h <= a.y);

  if (collision && info) {
    float overlapX = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
    float overlapY = std::min(a.y + a.h, b.y + b.h) - std::max(a.y, b.y);

    info->contactPoint = glm::vec2(
        std::max(a.x, b.x) + overlapX * 0.5f,
        std::max(a.y, b.y) + overlapY * 0.5f);

    glm::vec2 centerA(a.x + a.w * 0.5f, a.y + a.h * 0.5f);
    glm::vec2 centerB(b.x + b.w * 0.5f, b.y + b.h * 0.5f);
    glm::vec2 diff = centerB - centerA;

    if (overlapX < overlapY) {
      info->normal = glm::vec2(diff.x > 0 ? 1.0f : -1.0f, 0.0f);
      info->penetration = overlapX;
    } else {
      info->normal = glm::vec2(0.0f, diff.y > 0 ? 1.0f : -1.0f);
      info->penetration = overlapY;
    }
  }

  return collision;
}

bool CollisionManager::checkCircleCollision(const glm::vec2 &centerA, float radiusA,
                                            const glm::vec2 &centerB, float radiusB) const {
  float distance = glm::length(centerB - centerA);
  return distance <= (radiusA + radiusB);
}

glm::vec2 CollisionManager::calculateSeparationVector(const SDL_FRect &a, const SDL_FRect &b) const {
  float overlapX = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
  float overlapY = std::min(a.y + a.h, b.y + b.h) - std::max(a.y, b.y);

  glm::vec2 separation(0, 0);

  if (overlapX > 0 && overlapY > 0) {
    if (overlapX < overlapY) {
      separation.x = (a.x < b.x) ? -overlapX : overlapX;
    } else {
      separation.y = (a.y < b.y) ? -overlapY : overlapY;
    }
  }

  return separation;
}

float CollisionManager::calculatePenetrationDepth(const SDL_FRect &a, const SDL_FRect &b) const {
  float overlapX = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
  float overlapY = std::min(a.y + a.h, b.y + b.h) - std::max(a.y, b.y);

  return std::min(overlapX, overlapY);
}