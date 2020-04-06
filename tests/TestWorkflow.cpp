#include "catch.hpp"

#include <functional>
#include <map>
#include <string>

#include "step.hpp"
#include "workflow.hpp"

using namespace workflow;
using namespace std;

// Empty function
void g(void) {}

// This tests the output of the `get_nth_prime` function
TEST_CASE("simple workflow", "[workflow]") {

  SECTION("Empty workflow") { REQUIRE(makeWorkflow({}).size() == 0); }
  SECTION("One step") {
    Workflow w1 = makeWorkflow({{"a", "Step a", g, {}}});
    REQUIRE(w1.size() == 1);
    REQUIRE(w1["a"].id == "a");
  }
  SECTION("Linear steps") {
    Workflow w2 = makeWorkflow({{"a", "Step a", g, {}},
                                {"b", "Step b", g, {"a"}},
                                {"c", "Step c", g, {"b"}}});
    REQUIRE(w2.size() == 3);
    REQUIRE(step::viewDependencies(w2["b"]) == vector<string>{"a"});
    REQUIRE(step::viewDependencies(w2["c"]) == vector<string>{"b"});
  }
  SECTION("Invalid Workflow") {
    REQUIRE_THROWS(
        makeWorkflow({{"a", "Step a", g, {"b"}}, {"b", "Step b", g, {}}}));
  }
}