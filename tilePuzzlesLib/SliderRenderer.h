#ifndef _SLIDER_RENDERER_H_
#define _SLIDER_RENDERER_H_

#include "App.h"
#include "GLogger.h"
#include "TRenderer.h"
#include "SliderMesh.h"
#include "Tile.h"

#include <functional>
#include <iostream>
#include <vector>

using namespace filament;
using namespace filament::math;

namespace tilepuzzles {

struct SliderRenderer : TRenderer {

  SliderRenderer() {
    mesh = std::shared_ptr<Mesh>(new SliderMesh());
  }

  virtual void onMouseMove(const float2& dragPosition) {
  }

  virtual Tile* onMouseUp(const float2& pos) {
    Tile* tile = mesh->hitTest(app, pos);
    return tile;
  }

  virtual Tile* onMouseDown(const float2& pos) {
    Tile* tile = mesh->hitTest(app, pos);
    if (tile) {
      mesh->slideTiles(*tile);
      needsDraw = true;
    }
    return tile;
  }

  virtual void initMesh() {
    mesh->init(CFG);
  }

  static constexpr const char* CFG = R"({
    "type":"slider",
      "dimension": {
        "count": 24
      }    ,
    "border": {
      "top":1,
      "left":1,
      "width": 4,
      "height": 4
    }      
  })";
};

} // namespace tilepuzzles
#endif
