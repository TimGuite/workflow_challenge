#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <utility>

#include "cursor.hpp"
#include "step.hpp"

#ifndef ASYNCEXECUTOR_H_
#define ASYNCEXECUTOR_H_

namespace asyncexecutor {

enum ExecutionResult { success, failure };

struct ResultMessage {
  std::string id;
  ExecutionResult result;
};

ExecutionResult asyncExecutor(cursor::Cursor &,
                              std::function<bool(const step::Step &)>,
                              std::function<void(cursor::Cursor &)>);

void asyncStepExecutor(std::mutex &, std::mutex &, std::condition_variable &,
                       std::list<ResultMessage> &, step::Step &,
                       std::function<bool(const step::Step &)>);

} // namespace asyncexecutor

#endif