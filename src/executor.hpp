#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <utility>

#include "cursor.hpp"
#include "step.hpp"

#ifndef EXECUTOR_H_
#define EXECUTOR_H_

namespace executor {

enum ExecutionResult { success, failure };

ExecutionResult blockingExecutor(cursor::Cursor &,
                                 std::function<bool(const step::Step &)>,
                                 std::function<void(cursor::Cursor &)>);

ExecutionResult asyncExecutor(cursor::Cursor &,
                              std::function<bool(const step::Step &)>,
                              std::function<void(cursor::Cursor &)>);

void asyncStepExecutor(std::mutex &, std::mutex &, std::condition_variable &,
                       std::list<pair<std::string, ExecutionResult>> &,
                       step::Step &, function<bool(const step::Step &)>)

} // namespace executor

#endif