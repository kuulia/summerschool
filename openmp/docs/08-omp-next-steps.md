<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Next steps with OpenMP
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Next steps with OpenMP

# Next steps {.section}

# OpenMP programming best practices

- Explicitly declare variable privacy level (`shared`/`private`/`firstprivate`)
  - Avoid `shared` variables if not explicitly needed
  - *Default is `shared`!*
- Maximise parallel regions
  - Reduce fork-join overhead, e.g. combine multiple parallel loops into one large parallel region
  - Potential for better cache re-usage
- Parallelise outermost loops if possible
  - Move `parallel for / do` construct outside of inner loops
  - `collapse` may provide additional parallelism
- If using tasks: ensure all threads have a task to work on

# Recap / quiz

TODO: Menti access code / QR code

# OpenMP summary

- OpenMP is an application programming interface (API) for thread-based parallelisation
    - Compiler directives, runtime API, environment variables
    - Relatively easy to get started but specially efficient and/or real-world
      parallelisation non-trivial
- Features touched in this intro
    - Parallel regions, data-sharing attributes
    - Work-sharing, reductions, execution control
    - Tasks and task dependencies

# Things that we did not cover

- sections construct
- taskgroup and taskloop constructs
- simd construct
- constructs for loop transformations (tile, unroll, ...)
- ...

# Web resources

- OpenMP homepage: <http://openmp.org/>
- Good online tutorial: <https://computing.llnl.gov/tutorials/openMP/>
- More online tutorials: <http://openmp.org/wp/resources/#Tutorials>

