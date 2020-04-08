#include <map>
#include <string>
#include <vector>

#include "step.hpp"
#include "workflow.hpp"

#ifndef CURSOR_H_
#define CURSOR_H_

namespace cursor {

/*
Represents the possible state of the steps

Flow through the states should be one way
A state which is waiting can only become ready
A state which is ready can either complete or fail
The flow terminates at complete or fail

This should aid understanding for more complex workflows
*/
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