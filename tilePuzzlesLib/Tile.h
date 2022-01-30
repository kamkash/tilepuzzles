#ifndef _TILE_H_
#define _TILE_H_

#include "GLogger.h"
#include "Vertex.h"
#include "enums.h"

#include <math/vec2.h>

namespace tilepuzzles {

struct Tile {
  Tile(const std::string& id, const Point& topLeft, const Size& size, QuadVertices* pQuad,
       QuadIndices* pIndices, int texIndex, float texWidth, int indexOffset,
       const math::int2& gridCoord, int tileNum)
    : tileId(id), topLeft(topLeft), size(size), quadVertices(pQuad), gridCoord(gridCoord),
      quadIndicies(pIndices), tileNum(tileNum) {
    _initVertices(texIndex, texWidth);
    _initIndices(indexOffset);
  }

  Tile(const std::string& id) : tileId(id) {
  }

  void logVertices() const {
    L.info("TileId", tileId, "isBlank", isBlank);
    L.info("Grid Coord", gridCoord.x, gridCoord.y);
    std::for_each(std::begin(*quadVertices), std::end(*quadVertices),
                  [](const Vertex& v) {
                    L.info("pos:", v.position[0], v.position[1],
                           "texCoords:", v.texCoords[0], v.texCoords[1]);
                  });
  }

  void logIndices() const {
    L.info("TileId", tileId, "isBlank", isBlank);
    std::for_each(std::begin(*quadIndicies), std::end(*quadIndicies),
                  [](const uint16_t& idx) { L.info("index:", idx); });
  }

  void swap(Tile& other) {
    Point otherTopleft = other.topLeft;
    math::int2 otherGridCoord = other.gridCoord;
    other.topLeft = {topLeft.x, topLeft.y};
    topLeft = {otherTopleft.x, otherTopleft.y};
    other.updateVertices();
    updateVertices();

    other.gridCoord = {gridCoord.x, gridCoord.y};
    gridCoord = {otherGridCoord.x, otherGridCoord.y};
  }

  void translate(Direction dir, int maxCoord) {
    switch (dir) {
      case Direction::up:
        topLeft[1] += size[1];
        gridCoord.x -= 1;
        if (topLeft[1] > 1.) {
          topLeft[1] = -1. + size[1];
          gridCoord.x = maxCoord;
        }
        break;
      case Direction::down:
        topLeft[1] -= size[1];
        gridCoord.x += 1;
        if (topLeft[1] <= -1.) {
          topLeft[1] = 1.;
          gridCoord.x = 0;
        }
        break;
      case Direction::left:
        topLeft[0] -= size[0];
        gridCoord.y -= 1;
        if (topLeft[0] < -1.) {
          topLeft[0] = 1. - size[0];
          gridCoord.y = maxCoord;
        }
        break;
      case Direction::right:
        topLeft[0] += size[0];
        gridCoord.y += 1;
        if (topLeft[0] >= 1.) {
          topLeft[0] = -1.;
          gridCoord.y = 0;
        }
        break;
      default:
        break;
    }
    updateVertices();
  }

  bool onClick(const math::float2& coord) const {
    return (*quadVertices)[0].position.x <= coord.x &&
           (*quadVertices)[1].position.x >= coord.x &&
           (*quadVertices)[0].position.y <= coord.y &&
           (*quadVertices)[2].position.y >= coord.y;
  }

  Tile(Tile&&) = default;
  Tile(const Tile&) = default;

  void updateVertices() {
    // bottom left
    (*quadVertices)[0].position = {topLeft[0], topLeft[1] - size[1], 0.};

    // bottom right
    (*quadVertices)[1].position = {topLeft[0] + size[0], topLeft[1] - size[1], 0.};

    // top left
    (*quadVertices)[2].position = {topLeft[0], topLeft[1], 0.};

    // top right
    (*quadVertices)[3].position = {topLeft[0] + size[0], topLeft[1], 0.};

    // logVertices();
  }

  void updateTexCoords(int texIndex, float texWidth) {
    // bottom left
    (*quadVertices)[0].texCoords = {texWidth * texIndex, 0};

    // bottom right
    (*quadVertices)[1].texCoords = {texWidth * (texIndex + 1), 0};

    // top left
    (*quadVertices)[2].texCoords = {texWidth * texIndex, 1};

    // top right
    (*quadVertices)[3].texCoords = {texWidth * (texIndex + 1), 1};
  }

  void _initVertices(int texIndex, float texWidth) {
    updateVertices();
    updateTexCoords(texIndex, texWidth);
    // logVertices();
  }

  void _initIndices(int indexOffset) {
    (*quadIndicies)[0] = indexOffset;
    (*quadIndicies)[1] = indexOffset + 1;
    (*quadIndicies)[2] = indexOffset + 2;
    (*quadIndicies)[3] = indexOffset + 3;
    (*quadIndicies)[4] = indexOffset + 2;
    (*quadIndicies)[5] = indexOffset + 1;
  }

  Direction directionTo(Tile* other) {
    if (sameColumn(other)) {
      return other->gridCoord.x > gridCoord.x ? Direction::down : Direction::up;
    } else if (sameRow(other)) {
      return other->gridCoord.y > gridCoord.y ? Direction::right : Direction::left;
    } else {
      return Direction::none;
    }
  }

  bool sameRow(Tile* other) {
    return other->gridCoord.x == gridCoord.x;
  }

  bool sameColumn(Tile* other) {
    return other->gridCoord.y == gridCoord.y;
  }

  bool equals(Tile* other) {
    return tileId == other->tileId;
  }

  QuadVertices* quadVertices = nullptr;
  QuadIndices* quadIndicies = nullptr;
  Point topLeft;
  Size size;
  std::string tileId;
  int tileNum;
  bool isBlank = false;
  math::int2 gridCoord;

  constexpr static Logger L = Logger::getLogger();
};

} // namespace tilepuzzles

#endif