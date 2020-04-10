#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "cursor.hpp"
#include "executor.hpp"

using namespace cursor;
using namespace std;

namespace executor {

ExecutionResult blockingExecutor(Cursor &cur) {
  vector<string> readySteps = cur.readySteps();

  while (readySteps.size() > 0) {
    try {
      // Run the task in the first ready step
      cur.flow[readySteps[0]].task();
      cur.completed(readySteps[0]);
    }
    // Expect most functions to throw an exception with a message
    catch (char e) {
      cur.failed(readySteps[0]);
    }
    // Also need to catch any exception which is thrown
    catch (...)
    {
        cur.failed(readySteps[0]);
    }


    // Update the vector of ready steps
    readySteps = cur.readySteps();
  }

  // Check if all of the steps are completed
  bool all_complete = cur.completedSteps().size() == cur.flow.size();

  return (all_complete ? success : failure);
}

} // namespace executor