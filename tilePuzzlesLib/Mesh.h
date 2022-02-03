#ifndef _MESH_H_
#define _MESH_H_

#include "ConfigMgr.h"
#include "GLogger.h"
#include "GameUtil.h"
#include <cstdlib>
#include <ctime>

#include "App.h"
#include "TVertexBuffer.h"
#include "Vertex.h"
#include "Tile.h"
#include "enums.h"

#include <filament/Viewport.h>
#include <math/mat4.h>

using namespace std;
using namespace filament;
using namespace filament::math;

namespace tilepuzzles {

using TQuadVertexBuffer = TVertexBuffer<QuadVertices, QuadIndices, 4, 6>;

struct Mesh {

  Mesh() {
  }

  virtual ~Mesh() {
  }

  virtual void init(const std::string& jsonStr) {
    if (jsonStr.empty()) {
      configMgr.init();
    } else {
      configMgr.init(jsonStr);
    }
    initVertexBuffers();
    initTiles();
    initBorder();
  }

  virtual void initVertexBuffers() {
    vertexBufferBorder.reset(new TQuadVertexBuffer(4));
    const int tileCount = getTileCount();
    vertexBuffer.reset(new TQuadVertexBuffer(tileCount));
  }

  virtual Tile* const blankTile() {
    return nullptr;
  }

  virtual Direction canSlide(const Tile& tile) {
    return Direction::none;
  }

  virtual void slideTiles(const Tile& tile) {
  }

  virtual std::vector<Tile*> rollTiles(const Tile& tile, Direction dir) {
    return std::vector<Tile*>();
  }

  void logTiles() {
    std::for_each(std::begin(tiles), std::end(tiles), [](const Tile& t) {
      t.logVertices();
      t.logIndices();
    });
  }

  Tile* tileAt(int row, int column) {
    auto tileIter =
      std::find_if(tiles.begin(), tiles.end(), [row, column](const Tile& t) {
        return row == t.gridCoord.x && column == t.gridCoord.y;
      });
    if (tileIter != tiles.end()) {
      return &*tileIter;
    } else {
      return nullptr;
    }
  }

  Tile* hitTest(const App& app, const math::float2& viewCoord) {
    math::mat4 projMat = app.camera->getProjectionMatrix();
    math::mat4 invProjMat = app.camera->inverseProjection(projMat);
    float width = float(app.view->getViewport().width);
    float height = float(app.view->getViewport().height);
    math::float4 normalizedView = {viewCoord.x * 2. / width - 1.,
                                   viewCoord.y * -2. / height + 1., 0., 1.};
    math::float4 clipCoord = invProjMat * normalizedView;
    auto tileIter = std::find_if(tiles.begin(), tiles.end(), [&clipCoord](const Tile& t) {
      return t.onClick({clipCoord.x, clipCoord.y});
    });
    if (tileIter != tiles.end()) {
      return &*tileIter;
    } else {
      return nullptr;
    }
  }

  virtual int getTileCount() {
    return configMgr.config["dimension"]["count"].get<int>();
  }

  virtual void initTiles() {
    const int tileCount = getTileCount();
    const int dim = sqrt(tileCount);
    const float texWidth = 32. / 1024.;

    const Size size = {2. / dim, 2. / dim};
    Point topLeft = {-1., 1.};

    int t = 0;
    int indexOffset = 0;
    for (int r = 0; r < dim; ++r) {
      for (int c = 0; c < dim; ++c) {
        topLeft.y = 1. - r * size.y;
        topLeft.x = -1. + c * size.x;
        const std::string tileId = string("tile") + to_string(r) + to_string(c);
        Tile tile(tileId, topLeft, size, &vertexBuffer->get(t),
                  &vertexBuffer->getIndex(t), t, texWidth, indexOffset, {r, c}, t + 1);
        tiles.push_back(tile);
        ++t;
        indexOffset += 4;
      }
    }
  }

  void shuffle() {
    GameUtil::shuffle(tiles);
  }

  bool hasBorder() {
    return configMgr.config["border"] != nullptr;
  }

  virtual void initBorder() {
    auto border = configMgr.config["border"];
    if (border != nullptr) {
      const int borderTop = border["top"].get<int>();
      const int borderLeft = border["left"].get<int>();
      const int borderWidth = border["width"].get<int>();
      const int borderHeight = border["height"].get<int>();
      const int tileCount = getTileCount();
      const int dim = sqrt(tileCount);
      const float texWidth = 30. / 60.;
      const Size size = {2. / dim, 2. / dim};
      const float borderThickness = size.x * .1f;
      const Size horzSize = {size.x * borderWidth + borderThickness, borderThickness};
      const Size vertSize = {borderThickness, size.y * borderHeight + borderThickness};
      Point topLeft = {-1., 1.};

      // top
      topLeft.x = -1. + borderLeft * size.x;
      topLeft.y = 1. - borderTop * size.y;
      Tile topTile("borderTop", topLeft, horzSize, &vertexBufferBorder->get(0),
                   &vertexBufferBorder->getIndex(0), 0, texWidth, 0, {0, 0}, 1);
      borderTiles.push_back(topTile);

      // bottom
      topLeft.x = -1. + borderLeft * size.x;
      topLeft.y = 1. - (borderTop * size.y) - (borderHeight * size.y);
      Tile bottomTile("borderBottom", topLeft, horzSize, &vertexBufferBorder->get(1),
                      &vertexBufferBorder->getIndex(1), 0, texWidth, 4, {0, 0}, 2);
      borderTiles.push_back(bottomTile);

      // left
      topLeft.x = -1. + borderLeft * size.x;
      topLeft.y = 1. - borderTop * size.y;
      Tile leftTile("borderLeft", topLeft, vertSize, &vertexBufferBorder->get(2),
                    &vertexBufferBorder->getIndex(2), 1, texWidth, 8, {0, 0}, 3);
      borderTiles.push_back(leftTile);

      // right
      topLeft.x = -1. + (borderLeft * size.x) + (borderWidth * size.x);
      topLeft.y = 1. - borderTop * size.y;
      Tile rightTile("borderRight", topLeft, vertSize, &vertexBufferBorder->get(3),
                     &vertexBufferBorder->getIndex(3), 1, texWidth, 12, {0, 0}, 4);
      borderTiles.push_back(rightTile);
    }
  }

  ConfigMgr configMgr;
  std::shared_ptr<TQuadVertexBuffer> vertexBuffer;
  std::shared_ptr<TQuadVertexBuffer> vertexBufferBorder;
  std::vector<Tile> tiles;
  std::vector<Tile> borderTiles;
  Logger L;
};
} // namespace tilepuzzles
#endif