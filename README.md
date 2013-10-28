seta
====

Work-stealing scheduler for multithread parallel processing based on Cilk(1)(2).
Seta is structured as a library, providing the user with a set of functions to explicit
the parallelism. Seta is also an evaluator that computes information about the global spawn-tree
of a given computation and the allocated memory in order to prove the statements in the Cilk's theory.

[1] Robert D. Blumofe and Charles E. Leiserson. Scheduling multithreaded computations by work stealing.
[2] Robert D. Blumofe, Christopher F.Joerg, Brandley C. Kuszmaul, Charles E. Leiserson, Keith H.Randall, Yuli Zhou. Cilk: an efficient multithreaded runtime system.
