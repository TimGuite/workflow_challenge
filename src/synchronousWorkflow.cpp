/*
Example of a workflow being executed in a synchronous manner, with CLI input
to provide permission for manual processes
*/

#include <iostream>
#include <locale>
#include <string>

#include "cursor.hpp"
#include "executor.hpp"
#include "step.hpp"
#include "workflow.hpp"

using namespace cursor;
using namespace executor;
using namespace step;
using namespace workflow;
using namespace std;

bool cliSeekPermission(const Step &s) {
  string result;
  string in;
  while (result != "y" && result != "n") {
    cout << "**************" << endl;
    cout << "Step " << s.id << " seeking permission to run." << endl;
    cout << "Description: " << s.description << endl;
    cout << "Permission provided? (Y/N)" << endl;
    cin >> in;
    result = tolower(in[0]);
  }

  return (result == "y");
}

void onUpdate(cursor::Cursor &c) {
  // Print out information about the state of the workflow
  cout << "================" << endl;
  auto state = c.viewState();
  for (auto step : state) {
    cout << step.first << ": ";
    switch (step.second) {
    case waiting:
      cout << "waiting";
      break;
    case ready:
      cout << "ready";
      break;
    case fail:
      cout << "failed";
      break;
    case complete:
      cout << "complete";
      break;
    }
    cout << endl;
  }
}

// Empty function implementation
void f() {}

int main() {

  const Workflow w =
      makeWorkflow({{"A", "Add Reagent 1", f, manual, {}},
                    {"B", "Add Reagent 2", f, manual, {"A"}},
                    {"C", "Preheat Heater", f, automatic, {"B"}},
                    {"D", "Mix Reagents", f, automatic, {"B"}},
                    {"E", "Heat Sample", f, automatic, {"C", "D"}},
                    {"F", "Extract Sample", f, manual, {"E"}}});

  Cursor c{w};

  auto res = blockingExecutor(c, cliSeekPermission, onUpdate);

  if (res == success) {
    cout << "Execution completed successfully." << endl;
  } else {
    cout << "Execution failed." << endl;
  }

  return 0;
}