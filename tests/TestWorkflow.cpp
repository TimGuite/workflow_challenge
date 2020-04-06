#include "catch.hpp"

#include <functional>
#include <map>
#include <string>

#include "step.hpp"
#include "workflow.hpp"

using namespace workflow;
using namespace std;

// Emmpty function
void f(void) {}

// This tests the output of the `get_nth_prime` function
TEST_CASE("simple workflow", "[workflow]") {

  SECTION("Empty workflow") { REQUIRE(makeWorkflow().size() == 0); }
  SECTION("One step") {
    workflow w1 = makeWorkflow({{"a", "Step a", f, {}}});
    REQUIRE(w1.size() == 1);
    REQUIRE(w1["a"].id == "a");
  }
  SECTION("Linear steps") {
    workflow w2 = makeWorkflow({{"a", "Step a", f, {}},
                                {"b", "Step b", f, {"a"}},
                                {"c", "Step c", f, {"b"}}});
    REQUIRE(w2.size() == 3);
    REQUIRE(viewDependencies(w2["b"]) == vector<string>{"a"});
    REQUIRE(viewDependencies(w2["c"]) == vector<string>{"b"});
  }
  SECTION("Invalid Workflow") {
    REQUIRE_THROWS(
        makeWorkflow({{"a", "Step a", f, {"b"}}, {"b", "Step b", f, {}}}))
  }
}