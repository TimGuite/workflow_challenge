/*

Designed to test executor flow.

Will set up some workflows which are expected to work and some which are
expected to fail.

*/

#include "catch.hpp"

#include "cursor.hpp"
#include "executor.hpp"
#include "step.hpp"
#include "workflow.hpp"

using namespace workflow;
using namespace step;
using namespace cursor;
using namespace executor;

// Function which runs without problems
void successfulTask(void) {}
// Function which throws an exception while running
void problematicTask(void) { throw "An error"; }

bool successfulPermissionRequest() { return true; }
bool unsuccessfulPermissionRequest() { return false; }

// Empty on update function
void onUpdate(Cursor &c) {}

TEST_CASE("blocking executor", "[executor]") {
  SECTION("Simple workflow") {
    const Workflow w1 =
        makeWorkflow({{"a", "Step a", successfulTask, automatic, {}}});
    // Create a cursor from the workflow
    Cursor c1{w1};
    ExecutionResult r1 =
        blockingExecutor(c1, successfulPermissionRequest, onUpdate);
    REQUIRE(r1 == success);
  }

  SECTION("Steps with dependencies") {
    // Also has a manual step
    const Workflow w2 =
        makeWorkflow({{"a", "Step a", successfulTask, automatic, {}},
                      {"b", "Step b", successfulTask, automatic, {"a"}},
                      {"c", "Step c", successfulTask, manual, {"b"}}});
    Cursor c2{w2};
    ExecutionResult r2 =
        blockingExecutor(c2, successfulPermissionRequest, onUpdate);
    REQUIRE(r2 == success);
  }

  SECTION("Parallel Tasks") {
    const Workflow w3 = makeWorkflow(
        {{"a", "Add Reagent 2", successfulTask, automatic, {}},
         {"b", "Preheat Heater", successfulTask, automatic, {"a"}},
         {"c", "Mix Reagents", successfulTask, automatic, {"a"}},
         {"d", "Heat Sample", successfulTask, automatic, {"b", "c"}}});
    Cursor c3{w3};
    ExecutionResult r3 =
        blockingExecutor(c3, successfulPermissionRequest, onUpdate);
    REQUIRE(r3 == success);
  }

  SECTION("Parallel Tasks with error") {
    const Workflow w3 = makeWorkflow(
        {{"a", "Add Reagent 2", successfulTask, automatic, {}},
         {"b", "Preheat Heater", successfulTask, automatic, {"a"}},
         {"c", "Mix Reagents", problematicTask, automatic, {"a"}},
         {"d", "Heat Sample", successfulTask, automatic, {"b", "c"}}});
    Cursor c3{w3};
    ExecutionResult r3 =
        blockingExecutor(c3, successfulPermissionRequest, onUpdate);
    REQUIRE(r3 == failure);
  }

  SECTION("Permission not given") {
    const Workflow w4 =
        makeWorkflow({{"a", "Step a", successfulTask, manual, {}}});
    // Create a cursor from the workflow
    Cursor c4{w4};
    ExecutionResult r4 =
        blockingExecutor(c4, unsuccessfulPermissionRequest, onUpdate);
    REQUIRE(r4 == failure);
  }
}