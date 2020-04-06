#include <exception>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "workflow.hpp"

using namespace std;

namespace workflow {

Workflow makeWorkflow(vector<StepDescription> workflowDescription) {
  Workflow flow;
  for (auto s : workflowDescription) {
    // Check if the id has already been used
    if (flow.contains(s.id)) {
      throw "Ids for steps in the workflow must be unique";
    }
    // Create dependency list
    vector<reference_wrapper<step::Step>> dependencies;
    // Check if the dependencies have been read in
    for (auto dep : s.dependency_ids) {
      if (flow.contains(dep) == false) {
        throw "This step appears to depend on steps which have not yet been "
              "specified in the workflow";
      } else {
        dependencies.push_back(ref(flow[dep]));
      }
    }
    flow.insert({s.id, {s.id, s.description, s.task, dependencies}});
  }

  return flow;
}

} // namespace workflow
