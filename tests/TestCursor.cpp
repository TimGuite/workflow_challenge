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
    c1.completedSteps().size() == 0;
    c1.availableSteps().size() == vector<string>{"a"};

    // Mark task a as having completed
    c1.completed("a");
    c1.completedSteps == vector<string>{"a"};
    c1.availableSteps().size() == 0;
  }

  SECTION("Steps with dependencies") {
    const Workflow w2 = makeWorkflow({{"a", "Step a", h, {}},
                                      {"b", "Step b", h, {"a"}},
                                      {"c", "Step c", h, {"b"}}});
    Cursor c2{w2};

    c2.completedSteps().size() == 0;
    c2.availableSteps() == vector<string>{"a"};

    c2.completed("a");
    c2.completedSteps() == vector<string>{"a"};
    c2.availableSteps() == vector<string>{"b"};

    c2.completed("b");
    c2.completedSteps() == vector<string>{"a", "b"};
    c2.availableSteps() == vector<string>{"c"};

    c2.completed("c");
    c2.completedSteps() == vector<string>{"a", "b", "c"};
    c2.availableSteps().size() == 0;
  }

  SECTION("Parallel Tasks") {
    const Workflow w3 = makeWorkflow({{"a", "Add Reagent 2", h, {}},
                                      {"b", "Preheat Heater", h, {"a"}},
                                      {"c", "Mix Reagents", h, {"a"}},
                                      {"d", "Heat Sample", h, {"b", "c"}}});
    Cursor c3{w3};

    c3.completedSteps().size() == 0;
    c3.availableSteps() == vector<string>{"a"};

    c3.completed("a");
    c3.completedSteps() == vector<string>{"a"};
    c3.availableSteps() == vector<string>{"b", "c"};

    c3.completed("b");
    c3.completedSteps() == vector<string>{"a, b"};
    c3.availableSteps() == vector<string>{"c"};

    c3.completed("c");
    c3.completedSteps() == vector<string>{"a, b, c"};
    c3.availableSteps() == vector<string>{"d"};

    c3.completed("d");
    c3.completedSteps() == vector<string>{"a", "b", "c", "d"};
    c3.availableSteps().size() == 0;
  }

  SECTION("Failed steps") {
    const Workflow w4 = makeWorkflow({{"a", "Step A", h, {}}});
    Cursor c4{w4};

    c4.failed("a");
    c4.availableSteps().size() == 0;
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