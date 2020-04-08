/*

The cursor should know which steps have been completed and
which have yet to run.

It should be able to identify steps which can be run.

It should provide some mechanism for recording steps
as being completed.

It should be able to mark a step as having failed.

*/

#include "catch.hpp"

#include <map>
#include <string>
#include <vector>

#include "cursor.hpp"
#include "workflow.hpp"

using namespace workflow;
using namespace cursor;
using namespace std;

// Empty function
void h(void) {}

TEST_CASE("simple cursor", "[cursor]") {
  SECTION("Simple Workflow") {
    const Workflow w1 = makeWorkflow({{"a", "Step a", h, {}}});
    // Create a cursor from the workflow
    Cursor c1{w1};
    // None of the steps have completed yet
    REQUIRE(c1.completedSteps().size() == 0);
    REQUIRE(c1.readySteps() == vector<string>{"a"});

    // Mark task a as having completed
    c1.completed("a");
    REQUIRE(c1.completedSteps() == vector<string>{"a"});
    REQUIRE(c1.readySteps().size() == 0);
  }

  SECTION("Steps with dependencies") {
    const Workflow w2 = makeWorkflow({{"a", "Step a", h, {}},
                                      {"b", "Step b", h, {"a"}},
                                      {"c", "Step c", h, {"b"}}});
    Cursor c2{w2};

    REQUIRE(c2.completedSteps().size() == 0);
    REQUIRE(c2.readySteps() == vector<string>{"a"});

    c2.completed("a");
    REQUIRE(c2.completedSteps() == vector<string>{"a"});
    REQUIRE(c2.readySteps() == vector<string>{"b"});

    c2.completed("b");
    REQUIRE(c2.completedSteps() == vector<string>{"a", "b"});
    REQUIRE(c2.readySteps() == vector<string>{"c"});

    c2.completed("c");
    REQUIRE(c2.completedSteps() == vector<string>{"a", "b", "c"});
    REQUIRE(c2.readySteps().size() == 0);
  }

  SECTION("Parallel Tasks") {
    const Workflow w3 = makeWorkflow({{"a", "Add Reagent 2", h, {}},
                                      {"b", "Preheat Heater", h, {"a"}},
                                      {"c", "Mix Reagents", h, {"a"}},
                                      {"d", "Heat Sample", h, {"b", "c"}}});
    Cursor c3{w3};

    REQUIRE(c3.completedSteps().size() == 0);
    REQUIRE(c3.readySteps() == vector<string>{"a"});

    c3.completed("a");
    REQUIRE(c3.completedSteps() == vector<string>{"a"});
    REQUIRE(c3.readySteps() == vector<string>{"b", "c"});

    c3.completed("b");
    REQUIRE(c3.completedSteps() == vector<string>{"a", "b"});
    REQUIRE(c3.readySteps() == vector<string>{"c"});

    c3.completed("c");
    REQUIRE(c3.completedSteps() == vector<string>{"a", "b", "c"});
    REQUIRE(c3.readySteps() == vector<string>{"d"});

    c3.completed("d");
    REQUIRE(c3.completedSteps() == vector<string>{"a", "b", "c", "d"});
    REQUIRE(c3.readySteps().size() == 0);
  }

  SECTION("Failed steps") {
    const Workflow w4 = makeWorkflow({{"a", "Step A", h, {}}});
    Cursor c4{w4};

    c4.failed("a");
    REQUIRE(c4.readySteps().size() == 0);
  }

  SECTION("Exceptions") {
    const Workflow w5 =
        makeWorkflow({{"a", "Step A", h, {}}, {"b", "Step B", h, {}}});
    Cursor c5{w5};

    // Cannot complete a step which is not in the workflow
    REQUIRE_THROWS(c5.completed("d"));
    // Same for failures
    REQUIRE_THROWS(c5.failed("d"));

    // Cannot mark the same thing as completed twice
    c5.completed("a");
    REQUIRE_THROWS(c5.completed("a"));
    // Nor can it now fail
    REQUIRE_THROWS(c5.failed("a"));
  }
}