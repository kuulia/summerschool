<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Parallel computing

1. Parallel computing is a central concept in HPC. Which of the following statements are true?

- In parallel computing a problem is split into a smaller subproblems (TRUE)
- Parallel computing can be used only in problems where separately computed subtasks produce results that are meaningful as such. In other words, the results of the
subtasks need not be combined (FALSE)
- Parallel computing makes computations faster (TRUE)
- Parallel computing enables larger or more accuarete computations (TRUE)

2. The two extreme ends of parallel algorithms are trivially parallel algorithms and tightly coupled algorithms. Which of the statements below are correct?

- In trivially parallel algorithms there is very little (or no) interaction between subtasks (TRUE) 
- All the parallel algorithms are explicitly either trivially parallel or tightly coupled (FALSE)
- The task farm (or main/worker) approach is a common parallelization model, where one processing unit sends tasks to workers and then receives results back. This model works only if the tasks are computationally similar, and there is equal number of tasks and workers. (FALSE)
- Compared to trivially parallel algorithms tightly coupled algorithms are usually more difficult to implement in terms of programming (TRUE)

3. Below you’ll find a few statements about parallel scaling. Which of the statements are true?
- In parallel scaling the following rule is always true: The more processing units, the shorter the time to solve the problem (FALSE)
- Weak scaling is for solving problems faster (FALSE)
- There are several factors that can limit parallel scaling such as parallel overheads, extra operations, synchronization costs and load imbalance (TRUE)
- When using many parallel workers, it is easy and straightforward to avoid all the parallel scaling problems, because modern HPC programs can handle the parallelization automatically without any user intervention. (FALSE)

4. Below is scalability data from four different parallel programs. Which of the programs is the most efficient?

a. Strong scaling efficieny of 78 % from 10 to 100 nodes
b. Weak scaling efficiency of 88 % from 6 to 600 nodes
c. Speedup of 7x from 8 to 64 nodes
d. Strong scaling effiecieny of 55 % from 4 to 8 nodes

Trick question: no conclusion can be made even about the parallel efficiency of different programs (except 4 is probably the worst). Even more importantly, parallel scaling tells nothing about the absolute performance of a program, actually bad computational performance may lead into a good parallel scalability (parallel overheads are smaller in comparison to computations). In the examples here, the program 4 could have better performance (in terms of throughput, e.g. simulated nanoseconds per hour) with two nodes, than the program than the program 1 with 100 nodes.
 



 
