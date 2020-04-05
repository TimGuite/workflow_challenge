# Workflow Challenge


Consider the following 'workflow' -- a directed acyclic graph (dag) of 'steps'.

      Add Reagent 1
            |
            |
      Add Reagent 2
         /     \
        /       \
    Preheat     Mix
     Heater   Reagents
        \       /
         \     /
       Heat Sample
            |
            |
      Extract Sample 

This workflow specifies that the first reagent must be inserted into the device first, followed by the second reagent.
Then the heater may be preheated, and the reagents may be mixed.
These two steps may be done in parallel.
Once (and only once) both of these are done, the mixture may be heated, and finally extracted.

## Goals

1. Implement a data structure for representing a workflow in C++11 (or greater).
This should be designed to make it easy to define a particular workflow by hand.

2. Implement a 'cursor' which represents a position in a workflow. A new cursor, for instance, would have had no steps completed; as steps are ticked off, it would progress through the graph.
For example, a cursor may indicate that both reagents have been added and have been mixed, but the heater has not yet finished being preheated.
Once this is done, the cursor would move to the next step: heating the sample.

3. Distinguish two types of step: 'automatic' and 'manual'. Automatic steps are performed by the computer, whereas manual steps are performed by the user.
Example manual steps: adding reagents, and extracting the sample.
Example automatic steps: preheating the heater, mixing reagents, and heating the sample.

4. Have a 'process' maintain a cursor representing the current state of the device.
When an automatic step is reached, the process should ensure that the task is performed and is reported as done once it is complete.
You could make each of these tasks just sleep for some amount of time, or you could simulate a heater reaching a temperature, for example.
It is up to you.

5. When a manual step is reached, the user should be prompted with the instruction to perform that task and then asked to confirm that they have
done it.
This could be done on the command line, for example, or from a simple GUI.

Your solution should have an architecture that is unit-testable and should
include appropriate tests to ensure quality. 
