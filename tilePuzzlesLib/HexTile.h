#ifndef _HEX_TILE_H_
#define _HEX_TILE_H_

#include "GLogger.h"
#include "Tile.h"
#include "Vertex.h"
#include "enums.h"

#include <math/vec2.h>

namespace tilepuzzles {

struct HexTile : Tile {

  HexTile(const std::string& id, const Point& topLeft, const Size& size,
          TriangleVertices* pQuad, TriangleIndices* pIndices, int texIndex,
          float texWidth, int indexOffset, const math::int2& gridCoord, int tileNum)
    : Tile(id), triangleVertices(pQuad), triangleIndices(pIndices) {
    _initVertices(texIndex, texWidth);
    _initIndices(indexOffset);
  }

  virtual void _initIndices(int indexOffset) {
    (*triangleIndices)[0] = indexOffset;
    (*triangleIndices)[1] = indexOffset + 1;
    (*triangleIndices)[2] = indexOffset + 2;
  }

  virtual void updateVertices() {
    // top left
    (*triangleVertices)[0].position = {topLeft[0], topLeft[1], 0.};

    // top right
    (*triangleVertices)[2].position = {topLeft[0] + size[0], topLeft[1], 0.};

    // bottom
    (*triangleVertices)[2].position = {topLeft[0] + .5 * size[0], topLeft[1] - size[1], 0.};
  }

  virtual void updateTexCoords(int texIndex, float texWidth) {
    // bottom left
    (*triangleVertices)[0].texCoords = {texWidth * texIndex, 0};

    // bottom right
    (*triangleVertices)[1].texCoords = {texWidth * (texIndex + 1), 0};

    // top
    (*triangleVertices)[2].texCoords = {texWidth * texIndex, 1};
  }

  TriangleVertices* triangleVertices;
  TriangleIndices* triangleIndices;
  constexpr static Logger L = Logger::getLogger();
}; // namespace tilepuzzles

} // namespace tilepuzzles

#endif