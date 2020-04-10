#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <utility>

#include "cursor.hpp"
#include "step.hpp"

#ifndef ASYNCEXECUTOR_H_
#define ASYNCEXECUTOR_H_

namespace asyncexecutor {

enum ExecutionResult { success, failure };

ExecutionResult asyncExecutor(cursor::Cursor &,
                              std::function<bool(const step::Step &)>,
                              std::function<void(cursor::Cursor &)>);

void asyncStepExecutor(std::mutex &, std::mutex &, std::condition_variable &,
                       std::list<std::pair<std::string, ExecutionResult>> &,
                       step::Step &, std::function<bool(const step::Step &)>);

} // namespace asyncexecutor

#endif