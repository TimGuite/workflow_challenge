#include <map>
#include <string>
#include <vector>

#include "step.hpp"
#include "workflow.hpp"

#ifndef CURSOR_H_
#define CURSOR_H_

namespace cursor {

// Represents the possible state of the steps
enum StepState { ready, waiting, failed, completed };

class Cursor {
private:
  std::map<std::string, StepState> workflowState;

public:
  workflow::Workflow flow;

  Cursor Cursor(const &Workflow);

  std::vector<std::string> availableSteps();
  std::vector<std::string> completedSteps();
  void completed(std::string);
  void failed(std::string);
}

} // namespace cursor

#endif CURSOR_H