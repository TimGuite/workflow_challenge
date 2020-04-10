#include "cursor.hpp"

#ifndef EXECUTOR_H_
#define EXECUTOR_H_

namespace executor {

enum ExecutionResult { success, failure };

ExecutionResult blockingExecutor(cursor::Cursor &);

} // namespace executor

#endif