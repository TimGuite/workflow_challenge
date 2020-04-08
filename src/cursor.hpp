#include <map>
#include <string>
#include <vector>

#include "step.hpp"
#include "workflow.hpp"

#ifndef CURSOR_H_
#define CURSOR_H_

namespace cursor {

// Represents the possible state of the steps
enum StepState { waiting, ready, fail, complete };

class Cursor {
private:
  std::map<std::string, StepState> workflowState;
  void updateState();

public:
  workflow::Workflow flow;

  Cursor(const workflow::Workflow &);

  std::vector<std::string> readySteps();
  std::vector<std::string> completedSteps();
  void completed(std::string);
  void failed(std::string);
};

} // namespace cursor

#endif