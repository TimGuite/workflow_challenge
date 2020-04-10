#include <functional>

#include "cursor.hpp"

#ifndef EXECUTOR_H_
#define EXECUTOR_H_

namespace executor {

enum ExecutionResult { success, failure };

ExecutionResult blockingExecutor(cursor::Cursor &, std::function<bool(const step::Step &)>,
                                 std::function<void(cursor::Cursor &)>);

ExecutionResult asyncExecutor(cursor::Cursor &, std::function<bool(const step::Step &)>,
                                 std::function<void(cursor::Cursor &)>);

} // namespace executor

#endif