// Copyright 2017 Rick van Schijndel

#include <Arduino.h>
#include <stdio.h>

#include <Bounce2.h>
#include <U8g2lib.h>

#include "logo.h"
#include "player_dick.h"
#include "game_state.h"

#include "player.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

const uint8_t buttonPin = D3;
const uint8_t screenWidth = 128;
const uint8_t screenHeight = 64;

// Global game state
gameState game_state = start;

// The player in the game
Player player;

// The display object
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

Bounce button;

void setup(void) {
  u8g2.begin();
  u8g2.setFlipMode(0);
  u8g2.setFont(u8g2_font_artossans8_8r);
  pinMode(buttonPin, INPUT_PULLUP);
  button.attach(buttonPin);
  button.interval(10);
  Serial.begin(115200);
}

inline void nextGameState() {
  game_state = static_cast<gameState>((game_state + 1) % (gameOver + 1));
}

void drawPlayer(int y_position = 0) {
  int16_t y_position_from_beneath = screenHeight - player_height - y_position;
  int16_t player_y_pos_left = ((screenWidth / 3) - (player_width / 2));
  u8g2.drawXBM(
    player_y_pos_left,
    y_position_from_beneath,
    player_width,
    player_height,
    player_bits);
}

void drawObstacles() {
  // TODO: draw obstacles according to the values in some array
}


void drawBootupScreen(void) {
  int logoMiddleX = (screenWidth / 2) - (bootup_width / 2);
  int logoMiddleY = (screenHeight / 2) - (bootup_height / 2);
  u8g2.drawXBM(
    logoMiddleY,
    logoMiddleX,
    bootup_width,
    bootup_height,
    bootup_bits);
}

const int x_pos_gameover = 20;
const int y_pos_gameover = 20;
void drawGameOver(void) {
  u8g2.drawStr(x_pos_gameover, y_pos_gameover, "GAME OVER");
  // TODO: draw fancy asset?
}

const int x_pos_score = 20;
const int y_pos_score = 60;
const size_t kScoreBufferSize = 20;
void drawScore(void) {
  char scorebuffer[kScoreBufferSize];
  snprintf(scorebuffer, kScoreBufferSize, "Score: %u", player.getScore());
  u8g2.drawStr(x_pos_score, y_pos_score, scorebuffer);
}

const int x_pos_hiscore = 0;
const int y_pos_hiscore = 20;
void drawHiscoreScreen(void) {
  u8g2.drawStr(x_pos_hiscore, y_pos_hiscore, "HI SCORES");
  // TODO: draw hi scores
}

bool collisionDetected(void) {
  /*
  if player_right_x_position >= obstacle_left_x_position and
    player_left_x_position <= obstacle_right_x_position
    player_bottom_y_position <= obstacle_top_y_position
  */

  // TODO: implement algorithm, remove auto logic
  return player.getScore() > (unsigned)random(100, 250);
}

void updateObstaclePosition(void) {
  // TODO
}

void resetGame() {
  player = Player();
}

void loop(void) {
  // In the loop so it's only true once, after this it's false again.
  // This is so it won't think the button is pressed again every loop.
  bool buttonPressed = false;

  if (button.update()) {
    buttonPressed = button.read();
    Serial.println("Button update");
  }

  // clear buffer to start drawing
  u8g2.clearBuffer();

  switch (game_state) {
    case start:
      {
        drawBootupScreen();
        if (buttonPressed) {
          nextGameState();
        }
        break;
      }
    case hiscore:
      {
        drawHiscoreScreen();
        if (buttonPressed) {
          nextGameState();
        }
        break;
      }
    case play:
      {
        if (buttonPressed) {
          if (player.onGround()) {
            player.jump();
          }
        }

        player.updateYPosition();
        updateObstaclePosition();
        drawPlayer(player.getYPosition());
        drawObstacles();

        if (collisionDetected()) {
          nextGameState();
          break;
        }
        // happens after collision detection, so score will only get higher
        // if player does not collide
        player.updateScore();
        break;
      }
    case gameOver:
      {
        drawGameOver();
        drawScore();
        if (buttonPressed) {
          resetGame();
          nextGameState();
        }
        break;
      }
    default:
      {
        Serial.println("Invalid game state");
        break;
      }
  }
  // draw everything on the screen
  u8g2.sendBuffer();
}