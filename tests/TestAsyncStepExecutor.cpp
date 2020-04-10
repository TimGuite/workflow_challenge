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
      REQUIRE(queue.front().second == success);
    }
  }
}
