#include "catch.hpp"

#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <string>
#include <utility>

#include "asyncExecutor.hpp"
#include "cursor.hpp"
#include "step.hpp"
#include "workflow.hpp"

using namespace asyncexecutor;
using namespace cursor;
using namespace step;
using namespace workflow;
using namespace std;

// Function which runs without problems
void successfulTaskAsync(void) {}
// Function which throws an exception while running
void problematicTaskAsync(void) { throw "An error"; }

bool successfulPermissionRequestAsync(const Step &s) { return true; }
bool unsuccessfulPermissionRequestAsync(const Step &s) { return false; }

// Empty on update function
void onUpdateAsync(Cursor &c) {}

TEST_CASE("Asynchronous Step Executor", "[executor]") {
  SECTION("Execute a function successfully and see it on the queue") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<ResultMessage> queue;
    Step s1{"a", "Step a", successfulTaskAsync, automatic, {}};

    auto f1 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s1),
                    successfulPermissionRequestAsync);

    // Check that a successful result came back on the queue
    {
      unique_lock lk(queueM);
      queueCV.wait(lk);
      REQUIRE(queue.size() == 1);
      REQUIRE(queue.front().id == "a");
      REQUIRE(queue.front().result == success);
      // Indicates future has finished
      REQUIRE(f1.valid() == true);
    }
  }

  SECTION("Two asynchronous functions") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<ResultMessage> queue;
    Step s1{"a", "Step a", successfulTaskAsync, automatic, {}};
    Step s2{"b", "Step b", successfulTaskAsync, automatic, {}};

    auto f1 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s1),
                    successfulPermissionRequestAsync);
    auto f2 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s2),
                    successfulPermissionRequestAsync);

    // Check that a successful result came back on the queue
    {
      unique_lock lk(queueM);
      // Wait twice
      queueCV.wait(lk);
      // Can't be guaranteed that both steps will run first
      if (queue.size() == 1) {
        queueCV.wait(lk);
      }
      // Establish the values are different
      REQUIRE(queue.front().id != queue.back().id);
      // Check the results
      REQUIRE(queue.front().result == success);
      REQUIRE(queue.back().result == success);
      // Indicates future has finished
      REQUIRE(f1.valid() == true);
      REQUIRE(f2.valid() == true);
    }
  }

  SECTION("One success, one failure") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<ResultMessage> queue;
    Step s1{"a", "Step a", successfulTaskAsync, automatic, {}};
    Step s2{"b", "Step b", problematicTaskAsync, automatic, {}};

    auto f1 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s1),
                    successfulPermissionRequestAsync);
    auto f2 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s2),
                    successfulPermissionRequestAsync);

    // Check that a successful result came back on the queue
    {
      unique_lock lk(queueM);
      // Wait twice
      queueCV.wait(lk);
      // Can't be guaranteed that both steps will run first
      if (queue.size() == 1) {
        queueCV.wait(lk);
      }
      // Establish the values are different
      REQUIRE(queue.front().id != queue.back().id);
      // Check the results
      if (queue.front().id == "a") {
        REQUIRE(queue.front().result == success);
        REQUIRE(queue.back().result == failure);
      } else {
        REQUIRE(queue.back().result == success);
        REQUIRE(queue.front().result == failure);
      }
    }
  }

  SECTION("Two manual successes") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<ResultMessage> queue;
    Step s1{"a", "Step a", successfulTaskAsync, manual, {}};
    Step s2{"b", "Step b", successfulTaskAsync, manual, {}};

    auto f1 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s1),
                    successfulPermissionRequestAsync);
    auto f2 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s2),
                    successfulPermissionRequestAsync);

    // Check that a successful result came back on the queue
    {
      unique_lock lk(queueM);
      // Wait twice
      queueCV.wait(lk);
      // Can't be guaranteed that both steps will run first
      if (queue.size() == 1) {
        queueCV.wait(lk);
      }
      // Establish the values are different
      REQUIRE(queue.front().id != queue.back().id);
      // Check the results
      REQUIRE(queue.front().result == success);
      REQUIRE(queue.back().result == success);
    }
  }

  SECTION("One permission success, one permission failure") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<ResultMessage> queue;
    Step s1{"a", "Step a", successfulTaskAsync, manual, {}};
    Step s2{"b", "Step b", successfulTaskAsync, manual, {}};

    auto f1 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s1),
                    successfulPermissionRequestAsync);
    auto f2 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s2),
                    unsuccessfulPermissionRequestAsync);

    // Check that a successful result came back on the queue
    {
      unique_lock lk(queueM);
      // Wait twice
      queueCV.wait(lk);
      // Can't be guaranteed that both steps will run first
      if (queue.size() == 1) {
        queueCV.wait(lk);
      }
      // Establish the values are different
      REQUIRE(queue.front().id != queue.back().id);
      // Check the results
      if (queue.front().id == "a") {
        REQUIRE(queue.front().result == success);
        REQUIRE(queue.back().result == failure);
      } else {
        REQUIRE(queue.back().result == success);
        REQUIRE(queue.front().result == failure);
      }
    }
  }
}

// Same tests as above but for async executor
TEST_CASE("async executor", "[executor]") {
  SECTION("Simple workflow") {
    const Workflow w1 =
        makeWorkflow({{"a", "Step a", successfulTaskAsync, automatic, {}}});
    // Create a cursor from the workflow
    Cursor c1{w1};
    ExecutionResult r1 =
        asyncExecutor(c1, successfulPermissionRequestAsync, onUpdateAsync);
    REQUIRE(r1 == success);
  }

  SECTION("Steps with dependencies") {
    // Also has a manual step
    const Workflow w2 =
        makeWorkflow({{"a", "Step a", successfulTaskAsync, automatic, {}},
                      {"b", "Step b", successfulTaskAsync, automatic, {"a"}},
                      {"c", "Step c", successfulTaskAsync, manual, {"b"}}});
    Cursor c2{w2};
    ExecutionResult r2 =
        asyncExecutor(c2, successfulPermissionRequestAsync, onUpdateAsync);
    REQUIRE(r2 == success);
  }

  SECTION("Parallel Tasks") {
    const Workflow w3 = makeWorkflow(
        {{"a", "Add Reagent 2", successfulTaskAsync, automatic, {}},
         {"b", "Preheat Heater", successfulTaskAsync, automatic, {"a"}},
         {"c", "Mix Reagents", successfulTaskAsync, automatic, {"a"}},
         {"d", "Heat Sample", successfulTaskAsync, automatic, {"b", "c"}}});
    Cursor c3{w3};
    ExecutionResult r3 =
        asyncExecutor(c3, successfulPermissionRequestAsync, onUpdateAsync);
    REQUIRE(r3 == success);
  }

  SECTION("Parallel Tasks with error") {
    const Workflow w3 = makeWorkflow(
        {{"a", "Add Reagent 2", successfulTaskAsync, automatic, {}},
         {"b", "Preheat Heater", successfulTaskAsync, automatic, {"a"}},
         {"c", "Mix Reagents", problematicTaskAsync, automatic, {"a"}},
         {"d", "Heat Sample", successfulTaskAsync, automatic, {"b", "c"}}});
    Cursor c3{w3};
    ExecutionResult r3 =
        asyncExecutor(c3, successfulPermissionRequestAsync, onUpdateAsync);
    REQUIRE(r3 == failure);
  }

  SECTION("Permission not given") {
    const Workflow w4 =
        makeWorkflow({{"a", "Step a", successfulTaskAsync, manual, {}}});
    // Create a cursor from the workflow
    Cursor c4{w4};
    ExecutionResult r4 =
        asyncExecutor(c4, unsuccessfulPermissionRequestAsync, onUpdateAsync);
    REQUIRE(r4 == failure);
  }

  SECTION("onUpdateAsync function runs correctly") {
    // Create a lambda function which updates a variable in the local scope
    // We would expect this to be called on every update, i.e every time a
    // step is run and the initial call
    int count = 0;
    auto f = [&count](Cursor &c) mutable { count = count + 1; };
    const Workflow w5 =
        makeWorkflow({{"a", "Step a", successfulTaskAsync, automatic, {}},
                      {"b", "Step b", successfulTaskAsync, automatic, {"a"}},
                      {"c", "Step c", successfulTaskAsync, manual, {"b"}}});
    Cursor c5{w5};
    asyncExecutor(c5, successfulPermissionRequestAsync, f);
    REQUIRE(count == 4);
  }
}