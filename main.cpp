#include <iostream>

#include <tilePuzzelsLib.h>

static GameContext gameContext;

int main(int argc, char **argv) {
  init(&gameContext);
  render();
  return 0;
}