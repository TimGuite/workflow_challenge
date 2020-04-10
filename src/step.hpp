#include <functional>
#include <string>
#include <vector>

#ifndef STEP_H_
#define STEP_H_

namespace step {

// Differentiate between manual or automatic tasks
enum FlowControl { manual, automatic };

// Step structure
struct Step {
  std::string id;
  std::string description;
  std::function<void(void)> task;
  FlowControl type;
  std::vector<std::reference_wrapper<Step>> dependencies;
};

// Return a list of dependencies for the step
std::vector<std::string> viewDependencies(Step &);
} // namespace step

#endif