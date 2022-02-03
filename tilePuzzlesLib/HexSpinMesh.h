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

using TriangleVertexBuffer = TVertexBuffer<TriangleVertices, TriangleIndices, 3, 3>;

struct HexSpinMesh : Mesh {
  HexSpinMesh() {
  }

  virtual void initVertexBuffers() {
    const int tileCount = getTileCount();
    vertexBuffer.reset(new TriangleVertexBuffer(tileCount));
  }

  virtual void initTiles() {
    const int tileCount = getTileCount();
    const int dim = sqrt(tileCount);
    const float texWidth = 32. / 1024.;

    const Size size = {2. / dim, 2. / dim};
    Point topLeft = {-1., 1.};
    HexTile tile("tile1", topLeft, size, &vertexBuffer->get(0), &vertexBuffer->getIndex(0), 0,
              texWidth, 0, {0, 0}, 1);
    tiles.push_back(tile);
  }

  std::shared_ptr<TriangleVertexBuffer> vertexBuffer;
  std::vector<HexTile> hexTiles;
};

} // namespace tilepuzzles

#endif
