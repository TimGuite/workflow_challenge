#include "catch.hpp"

#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <string>
#include <utility>

#include "executor.hpp"
#include "step.hpp"

using namespace executor;
using namespace step;
using namespace std;

// Function which runs without problems
void successfulTaskAsync(void) {}
// Function which throws an exception while running
void problematicTaskAsync(void) { throw "An error"; }

bool successfulPermissionRequestAsync(const Step &s) { return true; }
bool unsuccessfulPermissionRequestAsync(const Step &s) { return false; }

TEST_CASE("Asynchronous Step Executor", "[executor]") {
  SECTION("Execute a function successfully and see it on the queue") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<pair<string, ExecutionResult>> queue;
    Step s1{"a", "Step a", successfulTaskAsync, automatic, {}};

    auto f1 = async(launch::async, asyncStepExecutor, ref(ioM), ref(queueM),
                    ref(queueCV), ref(queue), ref(s1),
                    successfulPermissionRequestAsync);

    // Check that a successful result came back on the queue
    {
      unique_lock lk(queueM);
      queueCV.wait(lk);
      REQUIRE(queue.size() == 1);
      REQUIRE(queue.front().first == "a");
      REQUIRE(queue.front().second == success);
      // Indicates future has finished
      REQUIRE(f1.valid() == true);
    }
  }

  SECTION("Two asynchronous functions") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<pair<string, ExecutionResult>> queue;
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
      REQUIRE(queue.front().first != queue.back().first);
      // Check the results
      REQUIRE(queue.front().second == success);
      REQUIRE(queue.back().second == success);
      // Indicates future has finished
      REQUIRE(f1.valid() == true);
      REQUIRE(f2.valid() == true);
    }
  }

  SECTION("One success, one failure") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<pair<string, ExecutionResult>> queue;
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
      REQUIRE(queue.front().first != queue.back().first);
      // Check the results
      if (queue.front().first == "a") {
        REQUIRE(queue.front().second == success);
        REQUIRE(queue.back().second == failure);
      } else {
        REQUIRE(queue.back().second == success);
        REQUIRE(queue.front().second == failure);
      }
    }
  }

  SECTION("Two manual successes") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<pair<string, ExecutionResult>> queue;
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
      REQUIRE(queue.front().first != queue.back().first);
      // Check the results
      REQUIRE(queue.front().second == success);
      REQUIRE(queue.back().second == success);
    }
  }

  SECTION("One permission success, one permission failure") {
    mutex ioM;
    mutex queueM;
    condition_variable queueCV;
    list<pair<string, ExecutionResult>> queue;
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
      REQUIRE(queue.front().first != queue.back().first);
      // Check the results
     if (queue.front().first == "a") {
        REQUIRE(queue.front().second == success);
        REQUIRE(queue.back().second == failure);
      } else {
        REQUIRE(queue.back().second == success);
        REQUIRE(queue.front().second == failure);
      }
    }
  }
}
