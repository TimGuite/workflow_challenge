#include <functional>
#include <string>
#include <vector>

#ifndef STEP_H_
#define STEP_H_

namespace step {
// Step structure
struct Step {
  std::string id;
  std::string description;
  std::function<void(void)> task;
  std::vector<std::reference_wrapper<Step>> dependencies;
};

// Return a list of dependencies for the step
std::vector<std::string> viewDependencies(Step &);
} // namespace step

#endif