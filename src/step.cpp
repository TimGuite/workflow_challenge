#include <string>
#include <vector>

#include "step.hpp"

using namespace std;

namespace step {

vector<string> viewDependencies(Step &step) {
  vector<string> dependencyIds;
  for (Step &dep : step.dependencies) {
    dependencyIds.push_back(dep.id);
  }

  return dependencyIds;
}

} // namespace step