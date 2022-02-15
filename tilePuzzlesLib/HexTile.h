#ifndef _HEX_TILE_H_
#define _HEX_TILE_H_

#include "GLogger.h"
#include "GeoUtil.h"
#include "Tile.h"
#include "Vertex.h"
#include "enums.h"

#include <math/mat3.h>
#include <math/mat4.h>
#include <math/mathfwd.h>
#include <math/vec2.h>

namespace tilepuzzles {

struct HexTile : Tile {

  HexTile(const std::string& id, const Point& topLeft, const Size& size, TriangleVertices* pQuad,
          TriangleIndices* pIndices, int texIndex, float texWidth, int indexOffset,
          const math::int2& gridCoord, int tileNum)
    : Tile(id), triangleVertices(pQuad), triangleIndices(pIndices) {
    this->topLeft = topLeft;
    this->size = size;
    this->gridCoord = gridCoord;
    this->tileNum = tileNum;
    initVertices(texIndex, texWidth);
    logVertices();
    initIndices(indexOffset);
  }

  virtual void initIndices(int indexOffset) {
    (*triangleIndices)[0] = indexOffset;
    (*triangleIndices)[1] = indexOffset + 1;
    (*triangleIndices)[2] = indexOffset + 2;
  }

  virtual void updateVertices() {
    float shift = trunc(gridCoord.y / 3.);

    if ((int)shift % 2) {
      topLeft[1] -= size[1];
    }

    const math::float3 tri[] = {
      {topLeft[0], topLeft[1] - size[1], 0.},           // bottom left
      {topLeft[0] + size[0], topLeft[1] - size[1], 0.}, // bottom right
      {topLeft[0] + .5 * size[0], topLeft[1], 0.}       // top
    };

    const math::float3 offset = -1. * math::float3({topLeft[0] + size[0] * .5, topLeft[1] - size[1], 0.});
    const math::float3 yoffset = {0., size[1], 0.};

    math::float3 invTri[] = {GeoUtil::rotate(tri[0], math::F_PI, {0., 0., 1.}, offset),
                             GeoUtil::rotate(tri[1], math::F_PI, {0., 0., 1.}, offset),
                             GeoUtil::rotate(tri[2], math::F_PI, {0., 0., 1.}, offset)};

    invTri[0] = GeoUtil::translate(invTri[0], yoffset);
    invTri[1] = GeoUtil::translate(invTri[1], yoffset);
    invTri[2] = GeoUtil::translate(invTri[2], yoffset);

    if (gridCoord.x % 2) {
      if (gridCoord.y % 3 == 0) {
        (*triangleVertices)[0].position = invTri[0];
        (*triangleVertices)[1].position = invTri[1];
        (*triangleVertices)[2].position = invTri[2];
      } else if (gridCoord.y % 3 == 1) {
        (*triangleVertices)[0].position = tri[0];
        (*triangleVertices)[1].position = tri[1];
        (*triangleVertices)[2].position = tri[2];
      } else {
        (*triangleVertices)[0].position = invTri[0];
        (*triangleVertices)[1].position = invTri[1];
        (*triangleVertices)[2].position = invTri[2];
      }
    } else {
      if (gridCoord.y % 3 == 0) {
        (*triangleVertices)[0].position = tri[0];
        (*triangleVertices)[1].position = tri[1];
        (*triangleVertices)[2].position = tri[2];
      } else if (gridCoord.y % 3 == 1) {
        (*triangleVertices)[0].position = invTri[0];
        (*triangleVertices)[1].position = invTri[1];
        (*triangleVertices)[2].position = invTri[2];
      } else {
        (*triangleVertices)[0].position = tri[0];
        (*triangleVertices)[1].position = tri[1];
        (*triangleVertices)[2].position = tri[2];
      }
    }
  }

  virtual void updateTexCoords(int texIndex, float texWidth) {
    texIndex = texIndex % 32;
    (*triangleVertices)[0].texCoords = {texWidth * texIndex, 0};
    (*triangleVertices)[1].texCoords = {texWidth * (texIndex + .9), 0};
    (*triangleVertices)[2].texCoords = {texWidth * (texIndex + .9), .4};
  }

  virtual bool onClick(const math::float2& coord) const {
    return false;
  }

  virtual void logVertices() const {
    L.info("TileId", tileId, "isBlank", isBlank);
    L.info("Grid Coord", gridCoord.x, gridCoord.y);
    std::for_each(std::begin(*triangleVertices), std::end(*triangleVertices), [](const Vertex& v) {
      L.info("pos:", v.position[0], v.position[1], "texCoords:", v.texCoords[0], v.texCoords[1]);
    });
  }

  void logIndices() const {
    L.info("TileId", tileId, "isBlank", isBlank);
    std::for_each(std::begin(*triangleIndices), std::end(*triangleIndices),
                  [](const uint16_t& idx) { L.info("index:", idx); });
  }

  TriangleVertices* triangleVertices;
  TriangleIndices* triangleIndices;
  constexpr static Logger L = Logger::getLogger();
  int hexNum;
}; // namespace tilepuzzles

} // namespace tilepuzzles

#endif
