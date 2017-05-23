#ifndef GAME_LIB_GAME_GAME_H_
#define GAME_LIB_GAME_GAME_H_

#include "player.h"
#include "asset.h"
#include "state.h"

class Game {
 public:
  Game(Asset player_asset, Asset* object_assets, uint8_t object_assets_length);
  void Draw();
  void Update(bool button_pressed);
 private:
  Asset m_player_asset;
  Asset* m_object_assets;
  uint8_t m_object_assets_length;
  Player m_player;
  GameState m_state;
  void Start(bool button_pressed);
  void Hiscore(bool button_pressed);
  void Play(bool button_pressed);
  void GameOver(bool button_pressed);
  void NextGameState();
  void UpdateObstaclePositions();
  bool CollisionDetected();
};

#endif  // GAME_LIB_GAME_GAME_H_