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

  static constexpr const char* CFG = R"({
    "type":"slider",
      "dimension": {
        "count": 24
      }
  })";

};
} // namespace tilepuzzles
#endif