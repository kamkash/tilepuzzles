#ifndef _TVERTEX_BUFFER_H_
#define _TVERTEX_BUFFER_H_
#include "GLogger.h"
#include "Vertex.h"
#include <stdlib.h>

namespace tilepuzzles {

template <typename VertexShape, typename IndexShape, int vertsPerShape, int indexPerShape>
struct TVertexBuffer {
  TVertexBuffer(int numVertShapes) : numVertShapes(numVertShapes) {
    vertShapes = (VertexShape*)malloc(numVertShapes * sizeof(VertexShape));
    indexShapes = (IndexShape*)malloc(numVertShapes * sizeof(IndexShape));
    numVertices = numVertShapes * vertsPerShape;
    numIndices = numVertShapes * indexPerShape;
    size = sizeof(vertShapes) * numVertShapes;
    indexSize = sizeof(indexShapes) * numVertShapes;
  }

  virtual ~TVertexBuffer() {
    delete[] vertShapes;
    delete[] indexShapes;
  }

  virtual size_t getSize() {
    return size;
  }

  virtual size_t getIndexSize() {
    return indexSize;
  }

  const VertexShape* add(int index, const VertexShape& quad) {
    const VertexShape* oldValue = vertShapes + index;
    std::copy(std::begin(quad), std::end(quad), std::begin(*(vertShapes + index)));
    return oldValue;
  }

  VertexShape& get(int index) {
    return vertShapes[index];
  }

  IndexShape& getIndex(int index) {
    return indexShapes[index];
  }

  QuadVertices* cloneVertices() {
    VertexShape* clonedVertices = (VertexShape*)malloc(numVertShapes * sizeof(VertexShape));
    memcpy(clonedVertices, vertShapes, numVertShapes * sizeof(VertexShape));
    return clonedVertices;
  }

  VertexShape* vertShapes;
  IndexShape* indexShapes;
  int numVertShapes = 0;
  int numVertices = 0;
  int numIndices = 0;
  size_t size = 0;
  size_t indexSize = 0;
  constexpr static Logger L = Logger::getLogger();
};
} // namespace tilepuzzles
#endif
