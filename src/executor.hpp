#include "cursor.hpp"

#ifndef EXECUTOR_H_
#define EXECUTOR_H_

namespace executor {

enum ExecutionResult { success, failure };

ExecutionResult blockingExecutor(const cursor::Cursor &);

}

#endif