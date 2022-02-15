#define CATCH_CONFIG_PREFIX_ALL
#include "GLogger.h"
#include "GeoUtil.h"
#include "TestUtil.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <math/mat3.h>
#include <math/mat4.h>
#include <math/mathfwd.h>
#include <math/vec2.h>

using namespace filament;
using namespace tilepuzzles;

tilepuzzles::Logger L;

#define LOG_VERT(label, arr)                                                                                 \
  std::for_each(std::begin(arr), std::end(arr),                                                              \
                [](const math::float3& f) { L.info(label, f[0], f[1], f[2]); });

CATCH_TEST_CASE("Geo", "[geo]") {
  tilepuzzles::TestUtil::init_test();

  CATCH_SECTION("geo rotate") {
    const math::float3 tri[] = {{-1., 1., 0.}, {1., 1., 0.}, {0., 2.732, 0}};
    const math::float3 invTri[] = {GeoUtil::rotate(tri[0], math::F_PI, {0., 0., 1.}, {0., -tri[0][1], 0.}),
                                   GeoUtil::rotate(tri[1], math::F_PI, {0., 0., 1.}, {0., -tri[0][1], 0.}),
                                   GeoUtil::rotate(tri[2], math::F_PI, {0., 0., 1.}, {0., -tri[0][1], 0.})};

    LOG_VERT("tri", tri);
    LOG_VERT("invTri", invTri);
  }

  CATCH_SECTION("geo translate") {
    const math::float3 tri[] = {{-1., 1., 0.}, {1., 1., 0.}, {0., 2.732, 0}};
    const math::float3 transTri[] = {GeoUtil::translate(tri[0], {0., -tri[0][1], 0.}),
                                     GeoUtil::translate(tri[1], {0., -tri[0][1], 0.}),
                                     GeoUtil::translate(tri[2], {0., -tri[0][1], 0.})};

    LOG_VERT("tri", tri);
    LOG_VERT("transTri", transTri);
  }
}