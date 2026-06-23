<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Reduction
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Reduction in OpenMP


# OpenMP reduction {.section}

# Reduction

- Reduction is a common pattern, for example calculating dot product:

<div class="column">
```cpp
dot = 0.0
#pragma omp parallel shared(x,y,dot)
{
  #pragma omp for
  for(int i = 0; i < n; i++) {
    dot += x[i] * y[i];
  }
}
```
</div>
<div class="column">
```fortranfree
dot = 0.0
!$omp parallel shared(x,y,dot)
  !$omp do
  do i = 1, n
    dot = dot + x(i)*y(i)
  end do
  !$omp end do
!$omp end parallel
```
</div>

- This code has a **race condition** as multiple threads update the shared `dot` variable simultanously
  - This race condition could be avoided by using the `atomic` construct, but that would be highly
    inefficient &rarr; use `reduction` clause instead


# Reduction clause

**`reduction(operator:var1, var2, ...)`**

- Performs reduction on the (scalar) variables

This is what happens under the hood:

- A private reduction variable is created for each thread's partial result
- This private variable is initialized to the operator's initial value
- After parallel region the reduction operation is applied to the private
  variables and the result is aggregated to the shared variable


# Reduction operators in C/C++

<div class="column">

| Operator | Initial value |
|----------|---------------|
| `+`      | `0`           |
| `-`      | `0`           |
| `*`      | `1`           |
| `max`    | least         |
| `min`    | largest       |
| `&&`     | `1`           |
| `||`     | `0`           |

</div>

<div class="column">

| Bitwise Operator | Initial value |
|----------|---------------|
| `&`      | `~0`          |
| `|`      | `0`           |
| `^`      | `0`           |

</div>


# Reduction operators in Fortran

<div class="column" style="font-size:0.8em">

| Operator         | Initial value |
|------------------|---------------|
| `+`              | `0`           |
| `-`              | `0`           |
| `*`              | `1`           |
| `max`            | least         |
| `min`            | largest       |
| `.and.`          | `.true.`      |
| `.or.`           | `.false.`     |
| `.eqv.`          | `.true.`      |
| `.neqv.`         | `.false.`     |

</div>

<div class="column" style="font-size:0.8em">

| Bitwise Operator | Initial value |
|------------------|---------------|
| `.iand.`         | all bits on   |
| `.ior.`          | `0`           |
| `.ieor.`         | `0`           |

</div>


# Race condition avoided with reduction clause

<div class="column">
```cpp
dot = 0.0
#pragma omp parallel \
    shared(x,y,dot) reduction(+:dot)
{
  #pragma omp for
  for(int i = 0; i < n; i++) {
    dot += x[i] * y[i];
  }
}
```
</div>
<div class="column">
```fortranfree
dot = 0.0
!$omp parallel shared(x,y,dot) reduction(+:dot)
  !$omp do
  do i = 1, n
    dot = dot + x(i)*y(i)
  end do
  !$omp end do
!$omp end parallel
```
</div>


# Summary {.section}

# Summary

- OpenMP has dedicated `reduction` clause to efficient calculation of parallel reductions
