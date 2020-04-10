#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "cursor.hpp"
#include "executor.hpp"
#include "step.hpp"

using namespace cursor;
using namespace step;
using namespace std;

namespace executor {

ExecutionResult
blockingExecutor(Cursor &cur,
                 function<bool(const step::Step &)> permissionRequest,
                 function<void(Cursor &)> onUpdate) {
  vector<string> readySteps = cur.readySteps();

  while (readySteps.size() > 0) {
    try {
      // Run the task in the first ready step
      if (cur.flow[readySteps[0]].type == automatic) {
        cur.flow[readySteps[0]].task();
        cur.completed(readySteps[0]);
      } else if (cur.flow[readySteps[0]].type == manual) {
        // Seek permission
        if (permissionRequest(cur.flow[readySteps[0]])) {
          // Execute if given
          cur.flow[readySteps[0]].task();
          cur.completed(readySteps[0]);
        } else {
          // Mark as failed if not
          cur.failed(readySteps[0]);
        }
      }
    }
    // Expect most functions to throw an exception with a message
    catch (char e) {
      cur.failed(readySteps[0]);
    }
    // Also need to catch any exception which is thrown
    catch (...) {
      cur.failed(readySteps[0]);
    }

    // Call the on update function
    onUpdate(cur);

    // Update the vector of ready steps
    readySteps = cur.readySteps();
  }

  // Check if all of the steps are completed
  bool all_complete = cur.completedSteps().size() == cur.flow.size();

  return (all_complete ? success : failure);
}

} // namespace executor