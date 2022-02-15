#ifndef _HEX_SPIN_MESH_H_
#define _HEX_SPIN_MESH_H_

#include "GLogger.h"
#include "HexTile.h"
#include "Mesh.h"
#include "TVertexBuffer.h"
#include "Vertex.h"

using namespace std;
using namespace filament;
using namespace filament::math;

namespace tilepuzzles {

struct HexSpinMesh : Mesh<TriangleVertexBuffer, HexTile> {
  HexSpinMesh() {
  }

  virtual int getTileCount() {
    int rows = configMgr.config["dimension"]["rows"].get<int>();
    int columns = configMgr.config["dimension"]["columns"].get<int>();
    return rows * 2 * columns * 3;
  }

  virtual void initVertexBuffers() {
    const int tileCount = getTileCount();
    vertexBuffer.reset(new TriangleVertexBuffer(tileCount));
  }

  virtual void initTiles() {
    const float sqrt3o2 = sqrt(3.) / 2.;
    int rows = configMgr.config["dimension"]["rows"].get<int>();
    int columns = configMgr.config["dimension"]["columns"].get<int>();
    const float texWidth = 32. / 1024.;
    int indexOffset = 0;
    const float a = 2. / columns / 2.;
    const float h = sqrt3o2 * a;
    const Size size = {a, h};
    Point topLeft = {-1., 1.};

    int t = 0;
    for (int r = 0; r < rows * 2; ++r) {
      for (int c = 0; c < columns * 3; ++c) {
        topLeft.x = -1. + c * a * .5;
        topLeft.y = 1. - r * h;
        const std::string tileId = string("tile") + to_string(r) + to_string(c);
        HexTile tile(tileId, topLeft, size, &vertexBuffer->get(t), &vertexBuffer->getIndex(t), t, texWidth,
                     indexOffset, {r, c}, t + 1);
        tiles.push_back(tile);
        indexOffset += 3;
        ++t;
      }
    }
  }
};

} // namespace tilepuzzles

#endif
