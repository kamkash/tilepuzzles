#ifndef _IO_UTIL_H_
#define _IO_UTIL_H_

#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <utils/Path.h>
#include <vector>

namespace tilepuzzles {
namespace IOUtil {

std::vector<unsigned char> loadBinaryAsset(const utils::Path& path) {
  std::ifstream input(path.c_str(), std::ios::binary);
  std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
  return std::move(buffer);
}
} // namespace IOUtil
} // namespace tilepuzzles
#endif