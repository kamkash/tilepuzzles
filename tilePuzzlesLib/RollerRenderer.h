#ifndef _ROLLER_RENDERER_H_
#define _ROLLER_RENDERER_H_

#include "App.h"
#include "GLogger.h"
#include "TRenderer.h"
#include "RollerMesh.h"
#include "Tile.h"

#include <iostream>
#include <thread>
#include <valarray>

using namespace filament;
using namespace filament::math;

namespace tilepuzzles {

struct RollerRenderer : TRenderer {

  RollerRenderer() {
    mesh = std::shared_ptr<Mesh>(new RollerMesh());
  }

  virtual void onMouseMove(const float2& dragPosition) {
    Tile* newTile = mesh->hitTest(app, dragPosition);
    if (newTile && !newTile->equals(dragTile)) {
      Direction dir = dragTile->directionTo(newTile);
      if (dir != Direction::none) {
        mesh->rollTiles(*dragTile, dir);
        needsDraw = true;
      }
    }
  }

  virtual Tile* onMouseDown(const math::float2& pos) {
    dragTile = mesh->hitTest(app, pos);
    return dragTile;
  }

  virtual Tile* onMouseUp(const math::float2& pos) {
    Tile* tile = mesh->hitTest(app, pos);
    return tile;
  }

  virtual void initMesh() {
    mesh->init(CFG);
  }

  static constexpr const char* CFG = R"({
    "type":"roller",
    "dimension": {
      "count": 25
    },
    "border": {
      "top":1,
      "left":1,
      "width": 4,
      "height": 4
    }
  })";

}; // namespace tilepuzzles

} // namespace tilepuzzles
#endif
