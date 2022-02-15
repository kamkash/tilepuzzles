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

struct HexSpinRenderer : TRenderer<TriangleVertexBuffer, HexTile> {

  HexSpinRenderer() {
    mesh = std::shared_ptr<Mesh<TriangleVertexBuffer, HexTile> >(new HexSpinMesh());
  }

  virtual void initMesh() {
    mesh->init(CFG);
  }  

  virtual void onMouseMove(const float2& dragPosition) {
  }

  virtual HexTile* onMouseDown(const math::float2& pos) {
    dragTile = mesh->hitTest(app, pos);
    if (dragTile) dragTile->logVertices();
    return dragTile;
  }

  virtual HexTile* onMouseUp(const math::float2& pos) {
    HexTile* tile = mesh->hitTest(app, pos);
    return tile;
  }  

  virtual Path loadTilesTexture() {
    L.info("Using root asset path ", FilamentApp::getRootAssetsPath());
    Path path = FilamentApp::getRootAssetsPath() + "textures/1-30color.png";
    return path;
  }  

  static constexpr const char* CFG = R"({
    "type":"HexSpinner",
      "dimension": {
        "rows": 5,
        "columns": 7
      }
  })";

};
} // namespace tilepuzzles
#endif