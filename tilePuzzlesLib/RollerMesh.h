#ifndef _ROLLER_MESH_H_
#define _ROLLER_MESH_H_

#include "GLogger.h"
#include "Mesh.h"
#include "enums.h"

#include "TQuadVertexBuffer.h"

using namespace std;
using namespace filament;
using namespace filament::math;

namespace tilepuzzles {

struct RollerMesh : Mesh {
  RollerMesh() {
  }

  virtual std::vector<Tile*> rollTiles(const Tile& tile, Direction dir) {
    auto rollerTiles = tilesToRoll(tile, dir);
    const int dim = sqrt(tiles.size());
    std::for_each(rollerTiles.begin(), rollerTiles.end(),
                  [dir, dim](Tile* t) { t->translate(dir, dim - 1); });

    return rollerTiles;
  }

  std::vector<Tile*> tilesToRoll(const Tile& tilePick, Direction dir) {
    const int pickRow = tilePick.gridCoord.x;
    const int pickCol = tilePick.gridCoord.y;
    const int dim = sqrt(tiles.size());
    auto rollerTiles = std::vector<Tile*>();
    switch (dir) {
      case Direction::down:
      case Direction::up: {
        for (int r = 0; r < dim; ++r) {
          rollerTiles.push_back(tileAt(r, pickCol));
        }
        return rollerTiles;
      }
      case Direction::left:
      case Direction::right: {
        for (int c = 0; c < dim; ++c) {
          rollerTiles.push_back(tileAt(pickRow, c));
        }
        return rollerTiles;
      }
      default:
        return rollerTiles;
    }
  }

}; // namespace tilepuzzles

} // namespace tilepuzzles
#endif