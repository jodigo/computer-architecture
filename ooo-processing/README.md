# Assignment: Out-of-order execution

## Table of Contents

* [Introduction](#introduction)
* [Tasks](#tasks)
  * [Task 1: Point and click](#task-1-point-and-click)
  * [Task 2: Register renaming](#task-2-a-rosehhhhregister-by-any-other-name)
  * [Task 3: Superscalar execution](#task-3-going-superscalar)
  * [Task 4: Load bypassing / forwarding](#task-4-load-early-load-often)
  * [Task 5: Analysis](#task-5-analysis)
* [Deliverables](#deliverables)


## Introduction

In this assignment you will modify a performance simulator to model a superscalar, out-of-order processor core, and measure the improvement in CPI.



## Tasks

In Tasks 1–4, you will implement a progressively more sophisticated simulator in `sim-ooo.c`. In Task 5, however, you will have to analyze _all_ of those variations, so you will either want to make the features you add conditional on flags or do Task 5 in parallel with Tasks 1–4. In your code, clearly label which part is for each task.


### Task 1: Point and click

The simulator baseline implemented in `sim-ooo.c` has an 8-entry fetch queue and a 32-entry unified reorder buffer + reservation station. The implementation has no support for tracking and resolving dependencies dynamically; instead, it dispatches instructions from the instruction fetch queue (IFQ) to the reorder buffer (ROB) only when the ROB is empty.

Evaluate the performance of this simulator by measuring the average CPI for each of the four benchmarks provided, and modify `results.csv` to record `task1_CPI`.


### Task 2: A rose^H^H^H^Hregister by any other name

Modify `sim-ooo.c` to implement an out-of-order core with register renaming and dynamic dependency tracking. You may assume that there are enough result storage slots for all instructions stored in the ROB, and that any operation in the ROB whose operands are ready can start execution provided its functional unit is available. You may also ignore structural hazards on the CDB if multiple functional units complete in the same cycle.

This means that you will need to add some kind of register alias table to rename incoming instructions, modify the ROB entry format to track active dependencies for each instruction, and resolve the dependencies dynamically as results become available.

To prevent incorrect load and store execution, your implementation will ensure that loads and stores are executed **in order** — that is, a load or store in the ROB may not make progress if there are any unfinished loads or stores earlier in the ROB.

Evaluate the performance of this simulator by measuring the average CPI for each of the four benchmarks provided, and record `task2_CPI` in `results.csv`.


### Task 3: Going superscalar

Modify `sim-ooo.c` to simulate a 2-way, 4-way, and 8-way superscalar processor. This means that you will have to fetch, dispatch, issue/execute, and retire multiple instructions at once. (The simulator can already execute any number of ROB entries at once, subject to functional unit availability.)

Make sure that your simulation still respects register dependencies and still executes memory references in order.

Evaluate the performance of this simulator by measuring the average CPI for each of the four benchmarks provided, and record in `task3_CPI_2`, `task3_CPI_4`, and `task3_CPI_8`.


### Task 4: Load early, load often

Modify `sim-ooo.c` to implement load bypassing and load forwarding, using an 8-entry store buffer. That means you will need to track _address_ dependencies as well as register dependencies, emulating the appropriate microarchitectural structure(s).

In this task, you will need to separately handle the time at which the effective address is available and the time at which the memory access has accessed its functional unit and returned load data or written store data to memory. Model the effective address as being available as soon as the dependencies of the relevant instruction are satisfied — that is, you do not need a separate functional unit to compute the address. Also, assume that your cache has enough MSHRs, so that the memory port is only occupied for as long as specified in the functional unit definition.

Pay attention to when the effective address is computed in the functional part of the simulator, and when it must be used to resolve address dependencies — make sure you don't use the effective address from another instruction.

Evaluate the performance of this simulator by measuring the average CPI for each of the four benchmarks provided for the scalar and superscalar variants, and record the CPI as `task4_CPI_1`, `task4_CPI_2`, `task4_CPI_4`, and `task4_CPI_8`.


### Task 5: Analysis

In `report.pdf`, describe the effect of the microarchitectural optimizations you implemented on the different benchmarks. What is the bottleneck in each case? Do some optimizations enable others or make them redundant? Which benchmarks benefit more from each optimization, and why?

To write the report, you will want to add counters to measure occupancy of the various structures you are modelling — the IFQ, the ROB, the store buffer, and the functional units — as well as measure the kinds of instructions being executed in each benchmark.


## Deliverables

### Task 1 [1 mark]

- modified `results.csv`

### Task 2 [3 marks]

- modified `sim-ooo.c`
- modified `results.csv`

### Task 3 [2 marks]

- modified `sim-ooo.c`
- modified `results.csv`

### Task 4 [3 marks]

- modified `sim-ooo.c`
- modified `results.csv`

### Task 5 [1 mark]

- modified `sim-ooo.c`
- `report.pdf`

