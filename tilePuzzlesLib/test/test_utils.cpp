#define CATCH_CONFIG_PREFIX_ALL
#include "GLogger.h"
#include "GameUtil.h"
#include "TestUtil.h"
#include <catch2/catch_test_macros.hpp>

using namespace tilepuzzles;

CATCH_TEST_CASE("GameUtil", "[game_utils]") {
  tilepuzzles::TestUtil::init_test();
  tilepuzzles::Logger L;

  GameUtil::init();

  CATCH_SECTION("random_50_50") {
    float rand = GameUtil::frand(1., 2.);
    L.info("frand", rand);
    L.info("coinFlip", GameUtil::coinFlip());
  }
}