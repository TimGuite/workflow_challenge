/*
Example of a workflow being executed in a synchronous manner, with CLI input
to provide permission for manual processes
*/

#include <iostream>
#include <locale>
#include <string>
#include <thread>

#include "cursor.hpp"
#include "step.hpp"
#include "syncExecutor.hpp"
#include "termcolor.hpp"
#include "workflow.hpp"

using namespace cursor;
using namespace syncexecutor;
using namespace step;
using namespace workflow;
using namespace std;
using namespace std::chrono_literals;

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
      cout << termcolor::white << "waiting";
      break;
    case ready:
      cout << termcolor::on_yellow << "ready";
      break;
    case fail:
      cout << termcolor::on_red << "failed";
      break;
    case complete:
      cout << termcolor::on_green << "complete";
      break;
    }
    cout << termcolor::reset << endl;
  }
}

// Empty function implementation
void f() { std::this_thread::sleep_for(5s); }

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
    cout << termcolor::on_green << "Execution completed successfully."
         << termcolor::reset << endl;
  } else {
    cout << termcolor::on_red << "Execution failed." << termcolor::reset
         << endl;
  }

  return 0;
}