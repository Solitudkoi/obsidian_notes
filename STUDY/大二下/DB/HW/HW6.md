7.1, 7.13, 7.21, 7.22, and 7.29

7.1 Suppose that we decompose the schema R = (A, B, C, D, E) into
(A, B, C)
(A, D, E).
Show that this decomposition is a lossless decomposition if the following set F of functional dependencies holds:
A → BC
CD → E
B → D
E → A

7.13 Show that the decomposition in Exercise 7.1 is not a dependency-preserving decomposition.

7.21 Give a lossless decomposition into BCNF of schema R of Exercise 7.1.

7.22 Give a lossless, dependency-preserving decomposition into 3NF of schema R of Exercise 7.1.


7.29 Show that the following decomposition of the schema R of Exercise 7.1 is not a lossless decomposition:
(A, B, C)
(C, D, E).
Hint: Give an example of a relation r(R) such that ΠA, B, C (r) ⋈ ΠC, D, E (r) ≠ r