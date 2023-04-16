#include "catch2/catch_test_macros.hpp"
#include "syringe.h"

TEST_CASE("Binding works", "Step-1") {
  REQUIRE(InjectorBuilder().bind(42).build().get<int>() == 42);
  std::function<int()> f = []() { return 42; };
  REQUIRE(InjectorBuilder().addFactory(f).build().get<int>() == 42);
}