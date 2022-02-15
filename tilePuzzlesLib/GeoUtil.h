#ifndef _GEO_UTIL_H_
#define _GEO_UTIL_H_

#include <math/mat3.h>
#include <math/mat4.h>
#include <math/mathfwd.h>
#include <math/vec2.h>

using namespace filament;
namespace tilepuzzles {
namespace GeoUtil {

math::float3 translate(const math::float3& pos, math::float3 offset) {
  math::mat4f trans1 = math::mat4f::translation(offset);
  trans1 = math::details::matrix::transpose(trans1);
  auto v1 = math::float4(pos, 1) * trans1;
  return {v1[0], v1[1], v1[2]};
}

math::float3 rotate(const math::float3& pos, float angle, math::float3 axis) {
  const math::mat4f rot = math::mat4f::rotation(angle, axis);
  auto v1 = math::float4(pos, 1) * rot;
  return {v1[0], v1[1], v1[2]};
}

math::float3 rotate(const math::float3& pos, float angle, math::float3 axis, math::float3 offset) {
  const math::mat4f rot = math::mat4f::rotation(angle, axis);
  math::mat4f trans1 = math::mat4f::translation(offset);
  trans1 = math::details::matrix::transpose(trans1);
  math::mat4f trans2 = math::mat4f::translation(-1 * offset);
  trans2 = math::details::matrix::transpose(trans2);
  math::mat4f mat = trans1 * rot * trans2;
  auto v1 = math::float4(pos, 1) * mat;
  return {v1[0], v1[1], v1[2]};
}

} // namespace GeoUtil
} // namespace tilepuzzles
#endif