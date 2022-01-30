#ifndef _TQUADVERTEX_BUFFER_H_
#define _TQUADVERTEX_BUFFER_H_
#include "GLogger.h"
#include "Vertex.h"
#include <stdlib.h>

namespace tilepuzzles {
struct TQuadVertexBuffer {
  TQuadVertexBuffer(int numQuads) : numQuads(numQuads) {
    quadVertices = (QuadVertices*)malloc(numQuads * sizeof(QuadVertices));
    quadIndicies = (QuadIndices*)malloc(numQuads * sizeof(QuadIndices));
    numVertices = numQuads * 4;
    numIndices = numQuads * 6;
    size = sizeof(QuadVertices) * numQuads;
    indexSize = sizeof(QuadIndices) * numQuads;
  }

  virtual ~TQuadVertexBuffer() {
    delete[] quadVertices;
    delete[] quadIndicies;
  }

  virtual size_t getSize() {
    return size;
  }

  virtual size_t getIndexSize() {
    return indexSize;
  }

  const QuadVertices* add(int index, const QuadVertices& quad) {
    const QuadVertices* oldValue = quadVertices + index;
    std::copy(std::begin(quad), std::end(quad), std::begin(*(quadVertices + index)));
    return oldValue;
  }

  QuadVertices& get(int index) {
    return quadVertices[index];
  }

  QuadIndices& getIndex(int index) {
    return quadIndicies[index];
  }

  QuadVertices* cloneVertices() {
    QuadVertices* clonedVertices = (QuadVertices*)malloc(numQuads * sizeof(QuadVertices));    
    memcpy(clonedVertices, quadVertices, numQuads * sizeof(QuadVertices));
    return clonedVertices;
  }

  QuadVertices* quadVertices;
  QuadIndices* quadIndicies;
  int numQuads = 0;
  int numVertices = 0;
  int numIndices = 0;
  size_t size = 0;
  size_t indexSize = 0;
  constexpr static Logger L = Logger::getLogger();
};
} // namespace tilepuzzles
#endif
