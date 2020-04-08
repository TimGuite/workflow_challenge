#include <exception>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "cursor.hpp"
#include "step.hpp"
#include "workflow.hpp"

using namespace std;

namespace cursor {

Cursor::Cursor(const workflow::Workflow &flow) : flow(flow) {
  // Insantiate the step state dictionary
  // We assume the workflows have been made with
  // makeWorkflow and are therefore safe from duplicate entries
  for (auto step_entry : flow) {
    if (step::viewDependencies(step_entry.second).size() == 0) {
      workflowState.insert(pair<string, StepState>{step_entry.first, ready});
    } else {
      workflowState.insert(pair<string, StepState>{step_entry.first, waiting});
    }
  }
}

vector<string> Cursor::availableSteps() {
  vector<string> available;
  for (auto step_entry : workflowState) {
    if (step_entry.second == ready) {
      available.push_back(step_entry.first);
    }
  }
  return available;
}

vector<string> Cursor::completedSteps() {
  vector<string> available;
  for (auto step_entry : workflowState) {
    if (step_entry.second == complete) {
      available.push_back(step_entry.first);
    }
  }
  return available;
}

void Cursor::completed(string id) {
  if (workflowState.contains(id) == false) {
    throw "Id to complete does not match step in the workflow";
  }
  if (workflowState.at(id) != ready) {
    throw "Id to complete was not ready";
  }
  workflowState[id] = complete;
}

void Cursor::failed(string id) {
  if (workflowState.contains(id) == false) {
    throw "Id to complete does not match step in the workflow";
  }
  if (workflowState.at(id) != ready) {
    throw "Id to complete was not ready";
  }
  workflowState[id] = fail;
}

} // namespace cursor