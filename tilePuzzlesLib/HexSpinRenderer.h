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
    mesh = std::shared_ptr<Mesh<TriangleVertexBuffer, HexTile>>(new HexSpinMesh());
  }

  virtual void initMesh() {
    mesh->init(CFG);
  }

  virtual HexTile* onRightMouseDown(const float2& viewCoord) {
    math::float3 clipCoord = normalizeViewCoord(viewCoord);
    HexTile* tile = mesh->hitTest(clipCoord);
    return tile;
  }

  virtual void onMouseMove(const float2& dragPosition) {
    if (dragTile) {
      math::float3 clipCoord = normalizeViewCoord(dragPosition);
      HexTile* newTile = mesh->hitTest(clipCoord);
      if (newTile && !newTile->equals(dragTile)) {
        float2 anchor = std::get<0>(dragAnchor);
        math::float3 anchVec = {dragTile->size.x, 0., 0.};
        math::float3 posVec = geo.translate(clipCoord, -1. * math::float3(anchor.x, anchor.y, 0.));
        math::float3 pNormal = geo.tcross(anchVec, posVec);

        float angle = 0.F;
        if (abs(pNormal.z) > HexTile::EPS && abs(pNormal.z - lastNormalVec.z) > HexTile::EPS) {
          if (posVec.y > anchVec.y) {
            if (pNormal.z > lastNormalVec.z) {
              // normal increase in top-right-left quadrant
              angle = posVec.x >= 0 ? -ROTATION_ANGLE : ROTATION_ANGLE;
            } else {
              // normal decrease in top-right-left quadrant
              angle = posVec.x >= 0 ? ROTATION_ANGLE : -ROTATION_ANGLE;
            }
          } else {
            if (pNormal.z > lastNormalVec.z) {
              // normal increase in bottom-right-left quadrant
              angle = posVec.x >= 0 ? -ROTATION_ANGLE : ROTATION_ANGLE;
            } else {
              // normal decrease in bottom-right-left quadrant
              angle = posVec.x >= 0 ? ROTATION_ANGLE : -ROTATION_ANGLE;
            }
          }
        }
        if (angle != 0.F) {
          mesh->rotateTileGroup(dragAnchor, angle);
          needsDraw = true;
        }
        lastNormalVec = pNormal;
      }
    }
  }

  virtual HexTile* onMouseDown(const math::float2& pos) {
    math::float3 clipCoord = normalizeViewCoord(pos);

    dragTile = mesh->hitTest(clipCoord);
    if (dragTile) {
      dragAnchor = mesh->nearestAnchorGroup({clipCoord.x, clipCoord.y});
      auto anchorPoint = std::get<0>(dragAnchor);
      math::float3 anchVec = {dragTile->size.x, 0., 0.};
      math::float3 posVec = geo.translate(clipCoord, -1. * math::float3(anchorPoint.x, anchorPoint.y, 0.));
      math::float3 pNormal = geo.tcross(anchVec, posVec);
      lastNormalVec = pNormal;
    }
    return dragTile;
  }

  virtual HexTile* onMouseUp(const math::float2& pos) {
    dragTile = nullptr;
    mesh->collectAnchors();
    math::float3 clipCoord = normalizeViewCoord(pos);
    HexTile* tile = mesh->hitTest(clipCoord);
    return tile;
  }

  void logDragAnchor() {
    auto pt = std::get<0>(dragAnchor);
    // std::vector<HexTile> tileGroup = std::get<1>(dragAnchor);
    L.info("anchorPoint", pt.x, pt.y);
    // std::for_each(tileGroup.begin(), tileGroup.end(), [](const HexTile& t) {
    // t.logVertices();
    // });
  }

  virtual Path loadTilesTexture() {
    L.info("Using root asset path ", FilamentApp::getRootAssetsPath());
    Path path = FilamentApp::getRootAssetsPath() + "textures/1-30color.png";
    return path;
  }

  std::tuple<math::float2, std::vector<HexTile>> dragAnchor;
  GeoUtil::GeoUtil geo;
  static constexpr float ROTATION_ANGLE = math::F_PI / 3.;
  static constexpr const char* CFG = R"({
    "type":"HexSpinner",
      "dimension": {
        "rows": 2,
        "columns": 2
      }
  })";
};
} // namespace tilepuzzles
#endif