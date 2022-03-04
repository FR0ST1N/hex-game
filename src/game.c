#include "include/game.h"

#include <raylib.h>
#include <stddef.h>

#define BOARD_SIZE 11

// Window globals
int width = 800;
int height = 450;
char title[] = "Hex";

// Game globals
enum State { kEmpty, kBlue, kRed };
typedef struct Hexagon {
  int x;
  int y;
  enum State state;
} Hexagon;
Hexagon board[BOARD_SIZE][BOARD_SIZE];
float radius = 0;
enum State player = kEmpty;
bool win = false;

void UpdateDrawFrame();
void DrawHexagon(int i, int j);
void InitBoard();
void DrawBoard();
void DrawBorder();
void DrawTextGroup();
void CheckRestartGame();
bool CheckWinState(int i, int j);
bool DidPlayerWin();

void Game() {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(width, height, title);

  // Radius to fit double the amount of board size
  radius = ((float)width / (BOARD_SIZE * 2)) / 2;

  InitBoard();

  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }

  CloseWindow();
}

void UpdateDrawFrame() {
  BeginDrawing();

  ClearBackground(RAYWHITE);

  CheckRestartGame();
  DrawTextGroup();
  DrawBorder();
  DrawBoard();

  EndDrawing();
}

void DrawTextGroup() {
  if (player == kRed) {
    DrawText(win ? "Red Won." : "Red's turn.", width - 200, 80, 20, RED);
  } else if (player == kBlue) {
    DrawText(win ? "Blue Won." : "Blue's turn.", width - 200, 80, 20, BLUE);
  } else {
    // Random turn at start of new game
    player = GetRandomValue(0, 1) == 0 ? kBlue : kRed;
  }
  DrawText("Hex", width - 200, 20, 50, BLACK);
  DrawText("Press R to restart game.", 20, height - 25, 15, DARKGRAY);
  if (win) {
    DrawText("Game Over", width - 200, 110, 30, player == kBlue ? BLUE : RED);
  }
}

void DrawBorder() {
  int n = BOARD_SIZE - 1;
  int extra = 15;
  Vector2 top_left = {board[0][0].x - radius - extra, board[0][0].y - radius};
  Vector2 bottom_left = {board[n][0].x - radius + extra - 8,
                         board[n][0].y + radius};
  Vector2 top_right = {board[0][n].x + radius - extra + 8,
                       board[0][n].y - radius};
  Vector2 bottom_right = {board[n][n].x + radius + extra,
                          board[n][n].y + radius};
  int thick = 3;
  DrawLineEx(top_left, bottom_left, thick, BLUE);
  DrawLineEx(top_right, bottom_right, thick, BLUE);
  DrawLineEx(top_left, top_right, thick, RED);
  DrawLineEx(bottom_left, bottom_right, thick, RED);
}

void CheckRestartGame() {
  if (IsKeyPressed(KEY_R)) {
    InitBoard();
    player = kEmpty;
    win = false;
  }
}

void DrawHexagon(int i, int j) {
  Hexagon *hexagon = &board[i][j];
  int sides = 6;
  int rotation = 0;
  Vector2 position = {hexagon->x, hexagon->y};
  bool is_empty = hexagon->state == kEmpty;
  if (is_empty && !win) {
    bool is_colliding =
        CheckCollisionPointCircle(position, GetMousePosition(), radius - 3);
    bool click = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    if (is_colliding && click) {
      // when player clicks highlighted hex set state and check for win
      hexagon->state = player;
      if (DidPlayerWin())
        win = true;
      else
        player = player == kBlue ? kRed : kBlue;
    } else if (is_colliding) {
      // highlight current hex
      DrawPoly(position, sides, radius, rotation, LIGHTGRAY);
    }
  } else if (!is_empty) {
    Color color = hexagon->state == kBlue ? BLUE : RED;
    DrawPoly(position, sides, radius, rotation, color);
  }
  DrawPolyLines(position, sides, radius, rotation, BLACK);
}

void InitBoard() {
  float x_offeset = 70;
  float y_offeset = 70;
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      board[i][j].x = x_offeset + (j * radius * 1.75);
      board[i][j].y = y_offeset + (i * radius * 1.50);
      board[i][j].state = kEmpty;
    }
    x_offeset += radius - 2;
  }
}

void DrawBoard() {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      DrawHexagon(i, j);
    }
  }
}

bool CheckWinState(int i, int j) {
  if (i < 0 || j < 0 || i >= BOARD_SIZE || j >= BOARD_SIZE ||
      board[i][j].state != player)
    return false;

  if ((player == kBlue && j == BOARD_SIZE - 1) ||
      (player == kRed && i == BOARD_SIZE - 1))
    return true;

  board[i][j].state = kEmpty;
  bool ans = CheckWinState(i + 1, j) || CheckWinState(i, j + 1) ||
             CheckWinState(i - 1, j) || CheckWinState(i, j - 1) ||
             CheckWinState(i + 1, j - 1) || CheckWinState(i - 1, j + 1);
  board[i][j].state = player;

  return ans;
}

bool DidPlayerWin() {
  bool ans = false;
  for (int i = 0; i < BOARD_SIZE; i++) {
    ans |= CheckWinState(player == kBlue ? i : 0, player == kRed ? i : 0);
  }
  return ans;
}
