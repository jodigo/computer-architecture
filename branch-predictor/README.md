# Assignment: Branch prediction

## Table of Contents

* [Introduction](#introduction)
* [Tasks](#tasks)
  * [Task 1](#task-1)
  * [Task 2](#task-2)
  * [Task 3](#task-3)
  * [Task 4](#task-4)
  * [Task 5](#task-5)
  * [Hints](#hints)
* [Deliverables](#deliverables)

## Introduction

In this assignment you model several branch predictors using a functional simulator as a starting point. You also try to design the best branch predictor you can for a given “area budget” and compete with your classmates on the accuracy of your branch predictor.

## Tasks

Implement the branch predictors listed below, using **no more than 262,144 bits of storage in total for all branch predictor FSM tables**. In practice, the global branch history register used in some of the predictors below requires some storage, but this is tiny compared to the branch predictor tables and you do not need to count this in the 262,144-bit limit.

For the branch prediction schemes below, measure the prediction accuracy on the four benchmarks provided to you (`go`, `gcc`, `fpppp`, `vpr`). Use the command line in the `README` file of each benchmark directory (this simulates 100M instructions).

You will implement all predictors by modifying `sim-safe.c` and surrounding your modifications with comments that identify code for each task. See [Deliverables](#deliverables) below for details.

### Task 1

Implement a **1-bit** predictor.

### Task 2

Implement a **2-bit** predictor using a **2-bit saturating counter** finite state machine (strongly NT <-> weakly NT <-> weakly T <-> weakly NT).

### Task 3

Implement a 1-bit **gselect** predictor with **1 bit** of global branch history.

### Task 4

Implement a 2-bit **gshare** predictor with **2 bits** of global branch history.

### Task 5

Implement a predictor of your choice that outperforms the Task 4 predictor. If you outperform 2-bit gshare _significantly_, you will be eligible for bonus points.

For this task only, you will also have to write a description of your predictor in `report-task5.md` (in Markdown format).

### Hints

- Which branches to track? You can only predict taken/not-taken for conditional branches, so you need to make sure you only track and count those.
- How do we know the actual outcome? If you examine how branches are implemented, you will observe that at some point the correct next PC is computed. Think about how you could tell whether a branch was taken or not if you had the current PC and the _correct_ next PC.
- How to address the branch predictor tables? Review the branch prediction slides to recall which bits of the PC we used and why.

## Deliverables

For each benchmark, measure the prediction accuracy of each predictor and report it by replacing `0` in the appropriate field of `results.csv`.

### Task 1 [2 marks]

- `sim-safe.c` with all modifications surrounded with lines `/* BEGIN TASK 1 */` and `/* END TASK 1 */`
- the appropriate fields of `results.csv`

### Task 2 [2 marks]

- `sim-safe.c` with all modifications surrounded with lines `/* BEGIN TASK 2 */` and `/* END TASK 2 */`
- the appropriate fields of `results.csv`

### Task 3 [2 marks]

- `sim-safe.c` with all modifications surrounded with lines `/* BEGIN TASK 3 */` and `/* END TASK 3 */`
- the appropriate fields of `results.csv`

### Task 4 [2 marks]

- `sim-safe.c` with all modifications surrounded with lines `/* BEGIN TASK 4 */` and `/* END TASK 4 */`
- the appropriate fields of `results.csv`

### Task 5 [2+ marks]

- `sim-safe.c` with all modifications surrounded with lines `/* BEGIN TASK 5 */` and `/* END TASK 5 */`
- `report-task5.md`
- the appropriate fields of `results.csv`
