#ifndef _HEX_SPIN_MESH_H_
#define _HEX_SPIN_MESH_H_

#include "GLogger.h"
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

  std::shared_ptr<TriangleVertexBuffer> vertexBuffer;
};

} // namespace tilepuzzles

#endif
