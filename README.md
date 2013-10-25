seta
====

Work-stealing scheduler for multithread parallel processing based on Cilk(1)(2).
Seta is structured as as library, providing the user with a set of functions to explicit
the parallelism. Seta is also an evaluator that computes information about the global spawn-tree
of a given computation and the allocated memory in order to prove the statements of Cilk theory.
