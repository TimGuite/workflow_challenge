#include "catch.hpp"

#include <functional>
#include <string>
#include <vector>

#include "step.hpp"

using namespace step;
using namespace std;

// Emmpty function
void f(void) {}

// This tests the output of the `get_nth_prime` function
TEST_CASE("simple steps", "[step]") {

  Step a{"a", "Step A", f, {}};
  REQUIRE(viewDependencies(a).size() == 0);
  // Depends on a
  Step b{"b", "Step B", f, {ref(a)}};
  REQUIRE(viewDependencies(b) == vector<string>{"a"});
  // Depends on a and b
  Step c{"c", "Step c", f, {ref(a), ref(b)}};
  REQUIRE(viewDependencies(c) == vector<string>{"a", "b"});
}