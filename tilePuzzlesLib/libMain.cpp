#include <nlohmann/json.hpp>
#include "GLogger.h"
#include "tilePuzzelsLib.h"
#include "TAppWin.h"

using namespace tilepuzzles;

static Logger L;
static TAppWin app;

void initLogger() {
  Logger::initLogger("tilePuzzlesLib");
  L = Logger::getLogger();
}

void initApp() {
  app.init();
}

void init() {
  initLogger();
  initApp();
}

void render() {
}
