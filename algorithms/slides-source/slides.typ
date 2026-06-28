#import "@preview/touying:0.7.3": *
#import themes.simple: *

#show math.equation.where(block: true): set text(size: 20pt)

#show link: set text(fill: blue)

#show: simple-theme.with(
  aspect-ratio: "16-9",
  header: none,
  //footer: [Ethan van Woerkom - CSC Summer School in High-Performance Computing 2026],
  config-common(show-notes-on-second-screen: none),
  config-colors(
  primary: black,
)
)

// _Lecture to be held at the Nuuksio CSC Summer School 2026 on Saturday 27.6.2026 from 9:00-10:00._
// _Lecture is 45 minutes + 15 minutes for questions._

#title-slide[
  = Parallel Algorithms
  CSC Summer School in High-Performance Computing 2026
  
  #v(2em)

  Saturday - the 27th of June, 2026.

  Author: Ethan van Woerkom (University of Helsinki)

  #v(2em)

  Licensed under the Creative Commons BY 4.0 License.
]

#speaker-note[
  During this summer school we have been learning how to program a supercomputer LUMI. We have been learning about multithreaded applications with OpenMP, and in particuar we have in detail discussed how to work the Message Parsing Interface (MPI). But there is one thing that we have not covered. Namely, how to make _good programs_.

  Now admittedly this quite a fuzzy concept. After all, there exist entire branches of software engineering dedicated to writing 'good' code, 'readable code', 'maintainable code', how to code in teams, etcetera. However all of these concepts assume that you already know the type of program that you want to implement. In this lecture, we will instead focus on the crucial thought process that should precede the writing of any piece of code, namely choosing the right algorithm to solve your computational problem.
]

== Learning Objectives
Student...
- ...can reason about when which computational methods to use.
- ...can break computational problems down into algorithms using flowcharts.
- ...can reason about the computational complexity of their code
- ...understands asymptotic complexity and big O notation.
- ...can identify when a problem is parallelisable, and when not.
- ...can name several well-known parallel algorithms.

= Part 1: Introduction
#speaker-note[
  - Transforming science problem, to mathematical problem, to computational problem. Algorithm is solution to a computational/mathematical problem.
  - What is an algorithm.
  - When do we need HPC/parallel computing?
]
== What is an Algorithm?
#speaker-note[
  Explain what an algorithm is: In other words, it is a problem solving recipe, and if you follow this recipe exactly step-by-step, then you will arrive at the solution to your problem.

  Stress that an algorithm is not the same as a program, but rather a crucial component of it.

  An algorithm is, if you will, the platonic ideal behind your program.
]


An algorithm is:
- _A finite sequence of instructions to solve a specific computational problem._
#pause

*NB:* Algorithm $!=$ Program
#pause

A program is:
- _A sequence of instructions._

== What is an Algorithm? - In Practice
#align(center)[
  #alternatives[#image("figures/whatisanalgorithm/0.pdf", height: 84%)][#image("figures/whatisanalgorithm/1.pdf", height: 84%)][#image("figures/whatisanalgorithm/2.pdf", height: 84%)][#image("figures/whatisanalgorithm/3.pdf", height: 84%)][#image("figures/whatisanalgorithm/4.pdf", height: 84%)][#image("figures/whatisanalgorithm/5.pdf", height: 84%)]
]

#speaker-note[
  Let's take our heat conduction problem as an example. #pause
  - A scientist or engineer needs to model heat flow for some reason. #pause
  - They cast this using the theory of conductivity into a 2D system of PDEs. #pause
  - They realise that this problem is not analytically tractable and that solving it requires numerical methods, so they convert it into a system of difference equations for us to solve. #pause
  - They knock on our door at CSC and ask us to solve this on Lumi, so we choose to solve this using a Domain Decomposition & row-by-row evaluation algorithm. #pause
  - Finally, we implement this using C++ and MPI for CPU/GPU/multi-node, what you will.
]

== Why care about Algorithms?

#speaker-note[
  Why care about algorithms though? You might say: _"I've been granted a million hours on LUMI, I know how to program the fastest GPUs in the world, and this summerschool taught me all the best optimisations. So does it really matter which algorithm I pick?"_

  Well, in the words of Brian Kernighan:
]

#quote(attribution: [_The Practice of Programming (Brian Kernighan & Rob Pike)_], block: true)[
  The study of algorithms and data structures is one of the foundations of computer
  science, a rich field of elegant techniques and sophisticated mathematical analyses.
  /*And it's more than just fun and games for the theoretically inclined:*/ [...] A good algorithm
  or data structure might make it possible to solve a problem in seconds that could otherwise take years. 
]

#speaker-note[
  Brian Kernighan is one of the original authors of the C programming language as well as a founding father of the Unix operating system. So, I would say he is a reliable source. However I am not asking you to take Brian Kernighan's word for it - rather, I am going to show to you right now that this is true.
]

== Why care about Algorithms: 2D Heat Equation (1/4)
// Slide:
// https://github.com/csc-training/summerschool/blob/main/mpi/exercises/heat-equation/code-description.md

#speaker-note[
  Let's take the heat flow problem from earlier.
  Suppose we take a very large problem size of 100,000x100,000 cells, and run wanted to know the temperature profile of your system after 10,000,000 timesteps. You write a perfect CPU+MPI implementation for Lumi working at peak performance. It would still take 100 seconds for all the nodes of the LUMI-C partition to solve this. However, if you choose a different algorithm, you can solve this problem with in only 10 seconds on my laptop. Sounds crazy? Let me show you how.
]

2D Discrete Heat Equation:

$
  u^(m+1)_(i,j) = u^m_(i,j) + (alpha Delta t)/(Delta x)^2 (u^m_(i-1, j) \- 2 u^m_(i, j) + u^m_(i+1, j)) + (alpha Delta t)/(Delta y)^2 (u^m_(i, j-1) - 2 u^m_(i, j) + u^m_(i, j+1))
$

#pause

Model large system: $10^5 times 10^5$ cells
- Want to know solution after $10^7$ timesteps.

#pause

$
  = 18 "FLOPs"\/"cell"\/"timestep" times (10^5)^2 "cells" times 10^7 "timesteps" = 1.8 times 10^18 "FLOPs"
$

LUMI-C Peak Performance: $10^16 "FLOP"\/s wide arrow.double.r wide 180 "seconds"$


== Why care about Algorithms? - 2D Heat Equation (2/4)
#speaker-note[
  We can solve this problem in Fourier space instead. We exploit the fact that our system of equations is linear and homogeneous in space, and so we can take the two-dimensional Discrete Fourier Transform of our initial conditions.
  
  Then we can analytically solve for the time evolution of the system mode-by-mode:
]

Alternatively, take Fourier transform...
$
    C^m_(k_x, k_y) = sum^(N_x)_(i=0) sum^(N_y)_(j=0) u^m_(i,j) e^(-2pi i (i dot k_x / N_x + j dot k_y / N_y)).
$

#pause

Solve for evolution of individual modes:
$
    C^(m_2)_(k_x, k_y) &= C^(m_1)_(k_x, k_y) dot (q_(k_x, k_y))^((m_2 - m_1)), wide q &= 1 - 4 alpha Delta t [(sin^2(2 pi k_x\/N_x))/(Delta x^2) + (sin^2(2 pi k_y\/N_y))/(Delta y^2)]
$

== Why care about Algorithms? - 2D Heat Equation (3/4)
Solve for evolution of individual modes:
$
    C^(m_2)_(k_x, k_y) &= C^(m_1)_(k_x, k_y) dot (q_(k_x, k_y))^((m_2 - m_1)), wide q &= 1 - 4 alpha Delta t [(sin^2(2 pi k_x\/N_x))/(Delta x^2) + (sin^2(2 pi k_y\/N_y))/(Delta y^2)]
$

Inverse Fourier Transform:

$
  u^m_(i,j) = sum^(N_x)_(k_x=0) sum^(N_y)_(k_y=0) C^m_(k_x, k_y) e^(2pi i (i dot k_x / N_x + j dot k_y / N_y)).
$

#speaker-note[
    Then finally, we can take the inverse Fourier transform to get our result back in real space.
]

== Why care about Algorithms? - 2D Heat Equation (4/4)
// Different algorithm:
// 10*10*(10^5)^2 = 10^12 FLOPs -> 10 sec on my laptop.
Summing up...
- Fourier Transform:
  - $approx 10 times (10^5)^2 log (10^5)^2 = 5 times 10^11 "FLOPs"$ #pause
- Mode Propagation:
  - $approx 100 "FLOPs"\/"Mode" times (10^5)^2 "Modes" = 10^12 "FLOPs"$ #pause
- Inverse Fourier Transform:
  - $approx 10 times (10^5)^2 log (10^5)^2 = 5 times 10^11 "FLOPs"$ #pause

In total: $~ 2 times 10^12 "FLOPs" wide arrow.double.r wide  10^6 times"speedup"$ 

$~20 "sec"$ on a personal computer ($~10^11 "GFLOP"\/"s"$).

== Why care about Algorithms? - The Take Away
#speaker-note[
  #set text(size: 20pt)
  In this problem we were able to leverage our phsical and mathematical knowledge to simplify the computational problem. Finally, using the smart Fast Fourier Transform (FFT) algorithm, we were able to make the problem tractable on a personal computer. It is clear that choosing the right algorithm for your problem can determine its computational feasability. Luckily computer science has already done this job for you. It is therefore crucial to be aware of what kinds of algorithms are out there to help you solve your problem.

  But HPC computing requires something more than choosing the right algorithm - to leverage the power of multi-core, multi-gpu, multi-node supercomputers we will need to find parallelisable algorithms to solve our problems.

  In the remainder of this talk I will first talk about how to describe algorithms, evaluate their performance, and then finally say a word about important parallel algorithms.
]

// Slide:
- Choice of algorithm can completely determine feasability of a problem
- In the case of heat equation we leveraged spectral with the Fast Fourier Transform to reduce computational cost.

#pause

#sym.arrow.double.r Always think about your algorithm before programming.

#pause
-  HPC computing is inherently parallel:
#pause
#sym.arrow.double.r Requires parallel algorithms.

= Part 2: Thinking about Algorithms
#speaker-note[
  - Flowcharts and Pseudocode
  - Computational complexity.
  - Asymptotic complexity.
  - Big O notation.
  - A word about datastructures
  - Example:
  - Bubblesort vs Quicksort.
]
== How to describe programs & algorithms
Tools to think about algorithms before writing code:

==== Pseudocode
   - Quick & easy to write
   - Can quickly become indistinguishable from real code.

==== Flowcharts
 - Visual tool
 - Forces you to think at a higher level of abstraction.

== Flowcharts
#grid(columns: (1.2fr, 1fr),
image("figures/flowchart-symbols.pdf", height: 84%),
[=== Charting Tools:
  - #link("https://www.drawio.com/")[plotly.io]
  - PureFlow (iOS)
  - Good old pen and paper
]
)

== Example Flowchart: Getting up
#align(center)[
  #alternatives[#image("figures/morning-flowchart/1.pdf", height: 84%)][#image("figures/morning-flowchart/2.pdf", height: 84%)][#image("figures/morning-flowchart/3.pdf", height: 84%)][#image("figures/morning-flowchart/4.pdf", height: 84%)][#image("figures/morning-flowchart/5.pdf", height: 84%)][#image("figures/morning-flowchart/6.pdf", height: 84%)][#image("figures/morning-flowchart/7.pdf", height: 84%)][#image("figures/morning-flowchart/8.pdf", height: 84%)][#image("figures/morning-flowchart/9.pdf", height: 84%)]
]
== Example Flowchart: 2D Heat Equation
#grid(columns: (1fr, 1fr),
alternatives[#image("figures/heat2d_1.pdf", height: 84%)]
,
[
  #pause
  #alternatives[#image("figures/heat2d_2.pdf", height: 84%)]
]
)

// == Example Flowchart: 2D Heat Equation (2/2)
// // Slide:
// // Initialise problem
// // FFT
// // For all coefficients: propagate
// // Inverse Fourier transform
// // Write to disk
// // end

== Computational Complexity
- Complexity is the amount of resources required to run an algorithm.

==== 2 Main kinds of Computational Complexity
#show strong: set text(fill: blue)
#grid(columns: (1fr, 1fr),
[
  *Time complexity* \
  #sym.tilde \# of operations in algorithm. 
],
[

  *Space complexity* \
  i.e. memory requirement.
])

==== Examples of an operation:
- Memory read/write
- Floating point operation

== Asymptotic Complexity
Describes how program performance scales on large problems

Expressed using "Big O notation":
- Program has asymptotic complexity $O(f(n))$:
- Then runtime is proportional to $f(n)$ (in the worst case).


Formally:

$
  T tilde O(f(N)) \ "if" space exists space k "s.t." \ T(N) < k dot f(N) "as" N arrow infinity
$ 

== Asymptotic Complexity (Example)

Suppose your program has time complexity:

$
  T(N) = 7 dot N^2 + 2 dot N + 3 "seconds" 
$

Then asymptotic complexity is:

$
  O(N^2) wide "(Big-O notation)"
$

Other variants of asymptotic complexity:
- Time or space complexity
- Worst/mean/best case complexity 
- Communications complexity etc...

== Asymptotic Complexity: Bubblesort
#align(center)[
  #grid(columns: 2,
  alternatives[#image("figures/bubblesort/1.pdf", height: 84%)][#image("figures/bubblesort/2.pdf", height: 84%)][#image("figures/bubblesort/3.pdf", height: 84%)][#image("figures/bubblesort/4.pdf", height: 84%)][#image("figures/bubblesort/5.pdf", height: 84%)][#image("figures/bubblesort/6.pdf", height: 84%)][#image("figures/bubblesort/7.pdf", height: 84%)][#image("figures/bubblesort/8.pdf", height: 84%)][#image("figures/bubblesort/9.pdf", height: 84%)],
    [
    #pause
    === Double Loop \ #sym.arrow \ Asymptotic Complexity: $O(N^2)$
  ]
  )
]

== Asymptotic Complexity: Quicksort
#align(center)[
  #grid(columns: 2,
  alternatives[#image("figures/quicksort/1.pdf", height: 84%)][#image("figures/quicksort/2.pdf", height: 84%)][#image("figures/quicksort/3.pdf", height: 84%)][#image("figures/quicksort/4.pdf", height: 84%)][#image("figures/quicksort/5.pdf", height: 84%)][#image("figures/quicksort/6.pdf", height: 84%)][#image("figures/quicksort/7.pdf", height: 84%)][#image("figures/quicksort/8.pdf", height: 84%)][#image("figures/quicksort/9.pdf", height: 84%)][#image("figures/quicksort/10.pdf", height: 84%)][#image("figures/quicksort/11.pdf", height: 84%)][#image("figures/quicksort/12.pdf", height: 84%)][#image("figures/quicksort/13.pdf", height: 84%)],
    [
    #pause
    === Recursive Divide & Conquer \ #sym.arrow \ Asymptotic Complexity: $O(N log N)$
  ]
  )
]

== Asymptotic Complexity: Overview
#place(left+horizon)[
#table(columns: (0.5fr, 0.5fr, 1.1fr),stroke: none,
[Notation],   [Name],         [Example],
table.hline(),
[*$O(1)$*],     [constant],     [array index],
[*$O(log N)$*], [logarithmic],  [binary search],
[*$O(N)$*],     [linear],       [string comparison],
[*$O(N log N)$*], [nlogn],      [quicksort, fast fourier transform],
[*$O(N^2)$*],   [quadratic],    [simple sorting methods],
[*$O(N^3)$*],   [cubic],        [matrix multiplication],
[*$O(2^N)$*],   [exponential],  [set partitioning, quantum problems],
)
]

#place(bottom)[#text(18pt)[Table based on p.41 of  _The Practice of Programming (Kernighan & Pike)_]]

== A Word about Datastructures
// Slide: A word about datastructures (choice of a good algorithm stands or falls with choice of your datastructure)

// From page 61 of Practice of Programming
#quote(attribution: [_The Mythical Man-Month (Frederick P. Brooks, Jr.)_], block: true)[
  Show me your flowcharts and conceal your tables, and I shall continue to be mystified. Show me your tables, and I won't usually need your flowcharts; they'll be obvious.
]

#pause
- Datastructure in memory matters equally as choice of algorithm.
- Example:
  - Add element to array: $O(N)$
  - Add element to linked list: $O(1)$

= Part 3: Parallel Algorithms
#speaker-note[
  - How to think about concurrency; flowcharts.
  - What is a parallel algoritm, communication bottleneck.
  - How concepts about asymptotic complexity transfer to parallel algorithms.
  - Curse and blessing of communications.
  - Embarassingly parallel.
  - Fundamentally serial problems.
  - Example:
  - Sorting re-visited.
]
== Introducing Concurrency
Parallel algorithms exploit _concurrency_:

#align(center)[  _concurrency_ = execution of several tasks simultaneously]

#pause

Can be used to:
- Reduce runtime
- Make larger problems possible

#pause

*NB:* Does *NOT* reduce time or space complexity itself.

== Potential for Parallelisation
For a given algorithm and problem:

#grid(columns: (0.05fr, 0.1fr, 1fr), gutter: 20pt,
[],[_Size_],[\= tot. no. of operations],
[],[_Width_],[\= min. no. of operations that can be executed concurrently],
[],[ _Depth_],[\= max. no. of operations that must be executed sequentially])

#grid(columns: (1fr, 1fr), gutter: 20pt,
[_Fundamentally Serial Problems:_],[$"Depth" ~ "Size"$],
[_Parallelisable Problems:_],[$"Depth" << "Size"$],
[_Embarassingly Parallel Problems:_],[ $"Depth" ~ 1$])
= Part 4: Common Parallel Algorithms in HPC

== Parallel Reduction

#grid(columns: 2,
image("figures/parallel-reduction.pdf", height: 80%),
[
  ==== Speedup:

  $
    S ~ p/(log p)
  $
]
)

== Parallel Grids: Domain Decomposition
For a grid consisting of tiles of size $N^D$:

- Time/Space Complexity \~ volume = $O(N^D)$

- Communication Complexity \~ surface area = $O(N^(D-1))$

- Communications-to-calculations ratio = $O(N^(-1))$

*NB:* Linear Speedup: spend more time on calculations than comms.

#sym.arrow Tile domain decomposition is a feasibly parallelisation strategy with linear speedup *if the tile size $N$ is sufficiently large.*

== The Fast Fourier Transform (FFT)

Discrete Fourier Transform (N d.o.f. #sym.arrow N modes):

$
    X_k = sum^(N)_(n=0) x_(j) e^(-2pi i (n dot k / N)).
$

Naive Fourier Transform: #h(1em) $O(N^2)$

FFT Time complexity: #h(2.3em) $O(N log N)$

\

Good for solving PDEs, see so-called _'spectral methods'_

== Parallel 1D FFT
// https://scispace.com/pdf/the-fft-on-a-gpu-3suuqfqijy.pdf
Difficult to parallelise:
- Uses global, highly non-local memory access patterns, 
- Typically communication bound, not compute-bound.
- Therefore limited to single node / single GPU
- Best to use specialised libaries\ (e.g. cuFFT)

Of course, parallel calculation of many smaller FFTs is straightforward.

== Distributed 2D FFT
FFTs are more easily parallised in higher dimensions, but still require global communications:
=== 2D FFT:

$
    X_(k_x, k_y) = sum^(N)_(n=0) sum^(M)_(m=0) x_(n, m) e^(-2pi i (n dot k / N + m dot l / M)).
$
Decompose into $N$ 1D FFTs of size N.


== Distributed 3D FFT
_Slab decomposition_: Distribute 3D FFT into $N$ 2D FFTs.

_Pencil decomposition_: Distribute 3D FFT into $N times N$ 1D FFTs.

#pause

No free lunch:

_Pencil decomposition_ :
- Is more parallelisable
- But requires more communications

== Today you learned...
#pause
...to think before you code.

#pause

...how to think about algorithms using flowcharts.

#pause

...about computational complexity and big $O(...)$ notation.

#pause

...how width and depth determines the parallelisability of a problem.

#pause

...about parallel algorithms such as 3D FFT and domain decomposition.

==== Resources:
- Chapter 2 of _The Principles of Programming_ (Kernigan & Pike)
