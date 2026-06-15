<!--
SPDX-License-Identifier: CC-BY-NC-SA-4.0

SPDX-FileCopyrightText: 2020 EPCC, The University of Edinburgh <https://www.epcc.ed.ac.uk/>
-->

<!-- Adapted from material by EPCC -->
<!-- https://github.com/EPCCed/archer2-MPI-2020-05-14 -->

# Summary of day 1

Questions can be copy-pasted e.g. to HackMd.



1. What is MPI?

   A. the Message Passing interface
   B. the Miami Police Investigators
   C. the Minimal Polynomial instantiation
   D. the Millipede Podiatry institution
   E. a way of doing distributed memory parallel programming

   A.
   B.
   C.
   D.
   E.


Correct: A, E

<!--
SPDX-SnippetBegin

SPDX-SnippetCopyrightText: 2020 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

2. How is a parallel MPI program executed?

   A. As a set of identical, independent processes
   B. Program starts serially, and then spawns and closes threads
   C. My MPI programs just crash :-(
   D. Each MPI task runs different program with different source code

   A.
   B.
   C.
   D.

Correct: A (even though in MPMD model D is also possible)

<!--
SPDX-SnippetEnd
-->


3. Compiling and running an MPI program requires

   A. special compilers
   B. special libraries
   C. a special parallel computer
   D. a special operating system
   E. a launcher program and runtime system

   A.
   B.
   C.
   D.
   E.


Correct: B, E

 MPI is a library and therefore requires no special compiler support:
 MPI “compilers“ such as mpicc/mpif90 are just convenient wrappers
 round standard compilers e.g. GNU, Intel or Cray that automatically
 set up appropriate include and link paths. MPI works by having
 multiple processes. Any modern OS can run multiple processes, even
 on a single core CPU.


4. After initiating an MPI program with "mpiexec -n 4
./my_mpi-program", what does the call to MPI_Init() do?

   A. create the 4 parallel processes
   B. start program execution
   C. enable the 4 independent programs subsequently to communicate with each other
   D. create the 4 parallel threads

   A.
   B.
   C.
   D.


Correct: C

The standard MPI launchers (mpiexec, mpirun, srun, aprun etc.) create
multiple copies of your MPI executable, each being a separate OS
process; there are already multiple processes running before the call
to MPI_Init. These will happily run independently - what the MPI_Init
call does is get them to talk to each other, which is required before
any user-initiated communications.

5. Which of the following are possible outputs from this piece of
    code run on 3 processes:

```
printf("Welcome from rank %d\n“, rank);
printf("Goodbye from rank %d\n", rank);
```

   A.
    ```
    Welcome from rank 0
    Welcome from rank 1
    Welcome from rank 2
    Goodbye from rank 0
    Goodbye from rank 1
    Goodbye from rank 2
	```
   B.
    ```
	Welcome from rank 2
    Welcome from rank 1
    Goodbye from rank 0
    Goodbye from rank 1
    Goodbye from rank 2
    Welcome from rank 0
	```
   C.
    ```
	Welcome from rank 2
    Goodbye from rank 2
    Welcome from rank 0
    Welcome from rank 1
    Goodbye from rank 1
    Goodbye from rank 0
	```
   D.
    ```
	Welcome from rank 0
    Goodbye from rank 1
    Welcome from rank 2
    Goodbye from rank 0
    Welcome from rank 1
    Goodbye from rank 2
	```

   A.
   B.
   C.
   D.


Correct: A, C

The output from different MPI processes is interleaved in an
unpredictable way. The order in which two print statements appear on
the screen has, in general, nothing to do with when the printf
statements actually happened. Remember that the output from many
separate programs running on many different computers is appearing on a
single screen - a lot of buffering etc. may take place between a print
statement executing and the output appearing on the screen in
particular, process x may execute a print statement before
process Y, but the output could appear in the other order (i.e. Y
appears before X ).

However, the ordering from an individual process will be maintained,
so for a particular process "X" the "Goodbye" from X will always
appear after the "Welcome" from X.

6. Which of the following statements do you agree with regarding this code:

```
for (i=0; i < size; i++)
 {
    if (rank == i)
      {
        printf("Hello from rank %d\n", rank);
        j = 10*i;
      }
 }
```

   A. The for loop ensures the operations are in order: rank 0, then rank
1, ...
   B. The for loop ensures the operation are done in parallel across all processes
   C. The for loop is entirely redundant
   D. The final value of j will be equal to 10*(size-1)

   A.
   B.
   C.
   D.


Correct: C

 Remember that every MPI process is executing the entire piece of code
 so each process only ever executes the if statement once when `rank ==
 i` The for loop is therefore
redundant, the above code produces exactly the same result as:

```
  printf("Hello from rank %d\n", rank);
  j = 10*rank;
```


7. If you call MPI_Recv and there is no incoming message, what happens?

   A. the Recv fails with an error
   B. the Recv reports that there is no incoming message
   C. the Recv waits until a message arrives (potentially waiting forever)
   D. the Recv times out after some system specified delay (e.g. a few
minutes)

   A.
   B.
   C.
   D.


Correct: C

MPI is not fault tolerant and assumes all processes are alive all the
time. There are therefore no timeouts in MPI. The assumption is that
you have written a correct program and that a matching message will
(eventually) arrive. This means that an unmatched receive waits
forever.

8. If you call MPI_Send and there is no matching receive, which of the
   following are possible outcomes?

A. the message disappears
   B. the send fails with an error
   C. the send waits until a receive is posted (potentially waiting forever)
   D. the message is stored and delivered later on (if possible)
   E. the send times out after some system specified delay (e.g. a few minutes)
   F. the program continues execution regardless of whether the message
is received

   A.
   B.
   C.
   D.
   E.
   F.

Correct: C, D

MPI_Send is blocking, so its completion *may* depend on the matching
receive. In practice, MPI will try to buffer small messages but,
rather than failing if there is insufficient buffer space, will switch
to using synchronous send for large messages. However, the threshold
to switch between the two is implementation dependent (and might even
change depending on how many processes you are running on) so you must
always handle the case that MPI_Send might be synchronous (or use
other MPI calls which will be discussed later on).

9. The MPI receive routine has a parameter "count". What does this mean?

   A. the size of the incoming message (in bytes)
   B. the size of the incoming message (in items, e.g. integers)
   C. the size of the buffer you have reserved tor storing the message in bytes
   D. the size of the buffer you have reserved for storing the message in items (e.g integers)

   A.
   B.
   C.
   D.


Correct: D

MPI tries to avoid talking about bytes - counting is almost always
done in number or items. For the receive, count is the size of the
local receive buffer, not of the incoming send buffer, although of
course in some programs they may be the same.

10. What happens if the incoming message is larger than “count"

   A. the receive fails with an error
   B. the receive reports zero data received
   C. the message writes beyond the end or the available storage
   D. only the first "count" items are received

   A.
   B.
   C.
   D.

Correct: A

MPI checks that the incoming message will fit into the supplied
storage before receiving it. The standard behaviour on error is for
the whole MPI program to exit immediately with a fatal error

10. What happens if the incoming message (of size "n") is smaller than "count"

   A. the receive fails with an error
   B. the receive reports zero data received
   C. the first "n" items are received
   D. the first "n" items are received and the rest of the storage is zeroed

   A.
   B.
   C.
   D.


Correct: C

This is entirely legal. In simple programs (e.g regular-grid
halo-swapping) you may know the size of all messages so #n" and
"count" might always be the same. However, in other situations
(e.g. particle-based codes) you may not know how many items are being
sent so you must ensure that you have enough storage locally

11. How is the actual size of the incoming message reported?

   A. the value of "count" in the receive is updated.
   B. MPI cannot tell you
   C. it is stored in the Status parameter
   D. via the associated tag

   A.
   B.
   C.
   D.


Correct: C

Various pieces of metadata about the received message are stored in
the Status such as the origin and tag (useful if you are using
wildcarding) and its size. Unfortunately, the size is not available
directly, you must pass the status to the helper routine MPI_Get_count.

<!--
SPDX-SnippetBegin

SPDX-SnippetCopyrightText: 2020 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

12. How can boundary MPI tasks *e.g.* in communication chain be treated?

   A. Using `MPI_PROC_NULL`
   B. Using `MPI_ANY_SOURCE`
   C. By putting MPI calls into "if-else" structures
   D. With the help of "modulo" operations

   A.
   B.
   C.
   D.

Correct: A, C, D

C is generally not recommended as code becomes often
cluttered. MPI_PROC_NULL is useful with finite chains where boundary
domains only send or receive. In periodic chains, modulo operations can
be used for wrapping destination and source ranks. (MPI_Cart_create is
even more convenient in most cases but not discussed in this course).

13. Which of the following statements apply to `MPI_Sendrecv`?

   A. It is required for correct functioning of MPI programs
   B. It is syntactic sugar in MPI
   C. It is useful preventing deadlocks
   D. It is useful for avoiding serialization of communication

   A.
   B.
   C.
   D.

Correct: C, D

Correctly functioning programs can be written just with MPI_Send and
MPI_Recv, however, combined Sendrecv can prevent deadlocks and improve
performance.

14. Which of the following statements apply to non-blocking communication?

   A. Communication happens in the background during computation
   B. Latency is smaller and bandwidth better than with blocking
   routines
   C. There is possibility for overlapping communication and computation
   D. Non-blocking routines can have a small performance penalty

   A.
   B.
   C.
   D.

Correct: C, D

Whether communication happens in the background during computation
depends on the MPI implementation and on the underlying hardware, but
non-blocking routines provide **possibility** for that. Communication
performance is not *per se* better than with blocking routines, and in
principle the treatment of requests incurs small overhead, although in
many cases negligible. However, by initiating several communications
(i.e. all directions in 3d halo exchange) one can in some cases
achieve higher aggregate bandwidth.

7. What is outcome of the following code snippet?
   ```cpp
   if (0 == myid) {
      int a = 4;
	  MPI_Isend(&a, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &req);
	  a = 6;
	  MPI_Wait(&req, MPI_STATUS_IGNORE);
   } else if (1 == myid) {
      int a;
	  MPI_Irecv(&a, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &req);
	  std::cout << "a is: " << a << std::endl;
      MPI_Wait(&req, MPI_STATUS_IGNORE);
  }
  ```

   A. 4
   B. 6
   C. Segmentation fault
   D. Not well defined

   A.
   B.
   C.
   D.


Correct: D

  The input buffer of `Isend` should not be modified before the
  communication is finalized with `Wait` or `Test` etc. Also, the
  output buffer of `Irecv` should not be read before receive is
  finalized. In practice, if the data was not received yet before
  printout, the outcome is 0 (or some random value if
  compiler did not initialize `a` to 0).  If the communication happened
  to take place before rank 0 modified `a`, outcome could be 4, or 6
  if rank 0 managed to modify `a` before communication was realized.

<!--
SPDX-SnippetEnd
-->
