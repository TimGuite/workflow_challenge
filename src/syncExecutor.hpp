#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <utility>

#include "cursor.hpp"
#include "step.hpp"

#ifndef SYNCEXECUTOR_H_
#define SYNCEXECUTOR_H_

namespace syncexecutor {

enum ExecutionResult { success, failure };

ExecutionResult blockingExecutor(cursor::Cursor &,
                                 std::function<bool(const step::Step &)>,
                                 std::function<void(cursor::Cursor &)>);

} // namespace syncexecutor

#endif