#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
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

// Function to asynchronously execute steps and push the result on to the queue
void asyncStepExecutor(mutex &ioMutex, mutex &queueMutex,
                       condition_variable &queueCV,
                       list<pair<string, ExecutionResult>> &messageQueue,
                       Step &s,
                       function<bool(const Step &)> permissionRequest) {

  ExecutionResult res;

  try {
    // Run the task in the first ready step
    if (s.type == automatic) {
      s.task();
      res = success;
    } else if (s.type == manual) {
      // Mutex scope
      {
        // Acquire I/O mutex
        std::scoped_lock<std::mutex> ioLock(ioMutex);
        // Seek permission
        bool permission = permissionRequest(s);
        // I/O lock released
      }
      if (permission) {
        // Execute if given
        s.task();
        res = success;
      } else {
        // Mark as failed if not
        res = failure;
      }
    }
  }
  // Expect most functions to throw an exception with a message
  catch (char e) {
    res = failure;
  }
  // Also need to catch any exception which is thrown
  catch (...) {
    res = failure;
  }

  // Mutex scope
  {
    // Acquire the queue mutex
    std::scoped_lock<std::mutex> queueLock(queueMutex);
    // Return the id and a result
    messageQueue.push_back({s.id, res});
    // Queue mutex released
  }
  // Notify the queueCV
  queueCV.notify_one();
}

ExecutionResult
asyncExecutor(Cursor &cur, function<bool(const step::Step &)> permissionRequest,
              function<void(Cursor &)> onUpdate) {

  bool all_complete = cur.completedSteps().size() == cur.flow.size();

  return (all_complete ? success : failure);
}

} // namespace executor