#include <map>
#include <string>
#include <vector>

#include "step.hpp"

namespace workflow {

//  Type alias for workflow
using Workflow = std::map<std::string, step::Step>;

struct StepDescription {
  std::string id;
  std::string description;
  std::function<void(void)> task;
  std::vector<std::string> dependency_ids;
};

Workflow makeWorkflow(std::vector<StepDescription>);

} // namespace workflow