#ifndef _HEX_SPIN_RENDERER_H_
#define _HEX_SPIN_RENDERER_H_

#include "App.h"
#include "GLogger.h"
#include "HexSpinMesh.h"
#include "TRenderer.h"
#include "Tile.h"

#include <functional>
#include <iostream>
#include <vector>

using namespace filament;
using namespace filament::math;

namespace tilepuzzles {

struct HexSpinRenderer : TRenderer {

  HexSpinRenderer() {
    mesh = std::shared_ptr<Mesh>(new HexSpinMesh());
  }

  virtual void initMesh() {
    mesh->init(CFG);
  }  

  virtual void onMouseMove(const float2& dragPosition) {
  }

  virtual Tile* onMouseDown(const math::float2& pos) {
    dragTile = mesh->hitTest(app, pos);
    return dragTile;
  }

  virtual Tile* onMouseUp(const math::float2& pos) {
    Tile* tile = mesh->hitTest(app, pos);
    return tile;
  }  

  static constexpr const char* CFG = R"({
    "type":"slider",
      "dimension": {
        "count": 1
      }
  })";

};
} // namespace tilepuzzles
#endif