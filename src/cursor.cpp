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

} // namespace cursor