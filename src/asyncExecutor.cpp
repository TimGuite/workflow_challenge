
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "asyncExecutor.hpp"
#include "cursor.hpp"
#include "step.hpp"

using namespace cursor;
using namespace step;
using namespace std;

namespace asyncexecutor {

// Function to asynchronously execute steps and push the result on to the queue
void asyncStepExecutor(mutex &ioMutex, mutex &queueMutex,
                       condition_variable &queueCV,
                       list<ResultMessage> &messageQueue, Step &s,
                       function<bool(const Step &)> permissionRequest) {

  ExecutionResult res;

  try {
    // Run the task in the first ready step
    if (s.type == automatic) {
      s.task();
      res = success;
    } else if (s.type == manual) {
      bool permission;
      // Mutex scope
      {
        // Acquire I/O mutex
        std::scoped_lock<std::mutex> ioLock(ioMutex);
        // Seek permission
        permission = permissionRequest(s);
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

  // It is necessary to keep a reference to a future to allow
  // it to execute in a truly asynchronous manner
  map<string, future<void>> runningSteps;

  // Thread related setup
  mutex ioMutex;
  mutex queueMutex;
  condition_variable queueCV;
  list<ResultMessage> queue;

  vector<string> readySteps = cur.readySteps();

  while (readySteps.size() > 0) {

    // Asynchronously call the asyncStepExecutor on the step and add it to the
    // map of running steps
    for (auto id : readySteps) {
      // Only put in steps which are not already running
      if (runningSteps.contains(id) == false) {
        runningSteps.insert(
            {id, async(launch::async, asyncStepExecutor, ref(ioMutex),
                       ref(queueMutex), ref(queueCV), ref(queue),
                       ref(cur.flow[id]), permissionRequest)});
      }
    }

    // Wait for messages to appear on the queue
    std::unique_lock<std::mutex> queueLock(queueMutex);
    queueCV.wait(queueLock);
    // Mutex has now been acquired
    while (queue.empty() == false) {
      // Process each message on the queue
      auto msg = queue.front();
      queue.pop_front();
      // Remove relevant future from runningSteps
      runningSteps.erase(msg.id);
      // Process the cursor
      if (msg.result == success) {
        cur.completed(msg.id);
      } else if (msg.result == failure) {
        cur.failed(msg.id);
      }
    }

    // Call the on update function
    onUpdate(cur);

    // Update the vector of ready steps
    readySteps = cur.readySteps();
  }

  bool all_complete = cur.completedSteps().size() == cur.flow.size();

  return (all_complete ? success : failure);
}

} // namespace asyncexecutor