- small_graph
```
Graph loaded. V = 10, initial cut = 9

===== Single Run Comparison =====
== Greedy ==
Cut Value: 24
Time: 0.000 ms
Final State: 0 1 1 0 0 1 1 0 0 1

== Single Flip ==
Cut Value: 22
Time: 0.000 ms
Final State: 1 0 0 1 0 0 1 1 0 1

== (2+eps) Big-Enough Improvement ==
Cut Value: 22
Time: 0.000 ms
Final State: 1 0 0 1 0 0 1 1 0 1

== Kernighan-Lin ==
Cut Value: 24
Time: 0.000 ms
Final State: 0 1 1 0 0 1 1 0 0 1

===== Repeated Time Measurement =====
== Greedy (repeat 1000 times) ==
Total Time: 2.000 ms
Average Time: 0.0020 ms

== Single Flip (repeat 1000 times) ==
Total Time: 2.000 ms
Average Time: 0.0020 ms

== Big Enough (esp=0.100000) (repeat 1000 times) ==
Total Time: 4.000 ms
Average Time: 0.0040 ms

== Kernighan-Lin (repeat 1000 times) ==
Total Time: 5.000 ms
Average Time: 0.0050 ms
```

- medium_sparse_graph
```
Graph loaded. V = 50, initial cut = 61

===== Single Run Comparison =====
== Greedy ==
Cut Value: 121
Time: 0.000 ms
Final State: 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1

== Single Flip ==
Cut Value: 103
Time: 0.000 ms
Final State: 0 1 0 1 1 0 1 0 1 0 1 0 1 0 1 0 0 1 1 0 0 1 1 0 0 1 1 0 1 0 1 0 0 1 0 1 0 1 0 1 1 0 0 1 1 0 0 1 1 0

== (2+eps) Big-Enough Improvement ==
Cut Value: 103
Time: 0.000 ms
Final State: 0 1 0 1 1 0 1 0 1 0 1 0 1 0 1 0 0 1 1 0 0 1 1 0 0 1 1 0 1 0 1 0 0 1 0 1 0 1 0 1 1 0 0 1 1 0 0 1 1 0

== Kernighan-Lin ==
Cut Value: 111
Time: 0.000 ms
Final State: 0 1 0 1 1 0 1 0 1 0 1 0 0 1 0 1 1 0 1 0 0 1 1 0 1 0 1 0 1 0 1 0 0 1 0 1 0 1 0 1 1 0 1 0 1 0 0 1 0 1

===== Repeated Time Measurement =====
== Greedy (repeat 1000 times) ==
Total Time: 24.000 ms
Average Time: 0.0240 ms

== Single Flip (repeat 1000 times) ==
Total Time: 21.000 ms
Average Time: 0.0210 ms

== Big Enough (esp=0.100000) (repeat 1000 times) ==
Total Time: 18.000 ms
Average Time: 0.0180 ms

== Kernighan-Lin (repeat 1000 times) ==
Total Time: 88.000 ms
Average Time: 0.0880 ms
```

- medium_dense_graph
```
Graph loaded. V = 50, initial cut = 123

===== Single Run Comparison =====
== Greedy ==
Cut Value: 170
Time: 0.000 ms
Final State: 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0

== Single Flip ==
Cut Value: 175
Time: 0.000 ms
Final State: 0 1 0 1 0 1 1 0 0 0 1 0 0 0 1 0 1 1 1 0 1 0 0 0 1 0 1 1 1 0 0 0 1 1 1 1 0 0 0 1 0 0 1 1 1 1 1 0 0 0

== (2+eps) Big-Enough Improvement ==
Cut Value: 176
Time: 0.000 ms
Final State: 1 1 0 0 1 1 1 0 0 0 1 0 0 0 1 0 1 1 1 0 1 0 0 0 1 0 1 1 1 0 0 0 1 1 1 1 0 0 0 1 0 0 1 1 1 1 1 0 0 0

== Kernighan-Lin ==
Cut Value: 180
Time: 0.000 ms
Final State: 0 0 1 1 0 1 0 0 0 1 1 0 0 0 1 0 1 1 1 0 1 0 0 0 1 0 1 1 1 0 0 1 1 1 0 1 0 0 0 1 0 1 1 1 0 0 1 1 1 0

===== Repeated Time Measurement =====
== Greedy (repeat 1000 times) ==
Total Time: 22.000 ms
Average Time: 0.0220 ms

== Single Flip (repeat 1000 times) ==
Total Time: 30.000 ms
Average Time: 0.0300 ms

== Big Enough (esp=0.100000) (repeat 1000 times) ==
Total Time: 34.000 ms
Average Time: 0.0340 ms

== Kernighan-Lin (repeat 1000 times) ==
Total Time: 112.000 ms
Average Time: 0.1120 ms
```


- large_sparse_graph
```
Graph loaded. V = 100, initial cut = 115

===== Single Run Comparison =====
== Greedy ==
Cut Value: 240
Time: 0.000 ms
Final State: 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1

== Single Flip ==
Cut Value: 221
Time: 0.000 ms
Final State: 0 1 0 1 0 1 0 1 1 0 1 0 1 0 1 0 1 0 1 1 0 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 1 0 0 1 0 1 0 1 1 0 1 1 0 1 0 0 1 0 1 0 0 1 1 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 1 0 0 1 0 1 0 1 0 1 1 0 1 0 1 0 1 1 0 1 0 1 0 1 0

== (2+eps) Big-Enough Improvement ==
Cut Value: 221
Time: 0.000 ms
Final State: 0 1 0 1 0 1 0 1 1 0 1 0 1 0 1 0 1 0 1 1 0 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 1 0 0 1 0 1 0 1 1 0 1 1 0 1 0 0 1 0 1 0 0 1 1 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 1 0 0 1 0 1 0 1 0 1 1 0 1 0 1 0 1 1 0 1 0 1 0 1 0

== Kernighan-Lin ==
Cut Value: 225
Time: 0.000 ms
Final State: 0 1 0 1 0 1 0 1 1 0 1 0 0 1 0 1 0 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 0 1 0 1 0 1 0 1 0 0 1 0 1 1 0 1 1 0 1 0 0 1 0 1 0 0 1 0 1 1 0 1 0 0 1 0 1 0 1 0 0 1 0 1 0 0 1 0 1 0 1 0 1 1 0 1 1 0 1 0 1 0 1 0 1 0 1 0

===== Repeated Time Measurement =====
== Greedy (repeat 1000 times) ==
Total Time: 77.000 ms
Average Time: 0.0770 ms

== Single Flip (repeat 1000 times) ==
Total Time: 85.000 ms
Average Time: 0.0850 ms

== Big Enough (esp=0.100000) (repeat 1000 times) ==
Total Time: 84.000 ms
Average Time: 0.0840 ms

== Kernighan-Lin (repeat 1000 times) ==
Total Time: 328.000 ms
Average Time: 0.3280 ms
```

- large_dense_graph
```
Graph loaded. V = 100, initial cut = 3704

===== Single Run Comparison =====
== Greedy ==
Cut Value: 4181
Time: 0.000 ms
Final State: 0 0 1 1 1 0 0 1 0 0 1 1 0 1 1 0 1 0 1 0 1 0 0 1 1 1 1 0 0 0 1 1 0 0 0 0 1 1 1 0 1 0 0 1 0 1 0 1 0 1 1 0 0 1 0 0 1 1 1 1 0 0 0 1 0 0 1 0 0 1 0 1 0 1 0 0 1 1 0 1 1 1 0 0 1 1 0 0 1 0 1 1 0 1 0 1 1 0 0 1

== Single Flip ==
Cut Value: 4419
Time: 5.000 ms
Final State: 1 0 0 0 1 0 1 1 0 1 0 0 1 1 0 1 1 1 1 1 1 1 0 0 1 1 0 0 0 0 0 1 1 0 1 1 1 0 0 1 1 0 0 0 0 0 0 0 0 1 1 1 0 1 0 0 1 0 1 1 0 0 1 1 1 0 0 0 1 0 1 1 0 0 0 1 0 1 0 0 1 1 1 1 0 0 1 1 0 0 1 1 1 0 1 0 0 1 1 0

== (2+eps) Big-Enough Improvement ==
Cut Value: 4276
Time: 2.000 ms
Final State: 0 0 0 1 0 0 0 0 0 1 1 0 1 1 0 0 1 1 1 1 1 1 1 1 1 0 0 0 1 0 0 0 1 1 1 0 1 0 0 0 1 0 0 0 1 0 0 1 1 0 1 0 0 1 0 1 1 1 1 0 0 1 0 0 0 0 0 0 1 0 1 0 1 1 0 0 1 1 0 0 1 0 1 0 1 0 1 1 1 1 1 1 1 0 1 0 0 1 1 1

== Kernighan-Lin ==
Cut Value: 4328
Time: 14.000 ms
Final State: 0 1 0 0 0 0 1 1 0 1 1 0 1 1 1 0 1 1 1 1 1 0 1 0 1 1 0 1 1 0 0 1 1 1 0 1 1 0 1 0 0 0 1 1 0 1 0 1 0 0 1 0 0 1 0 1 1 0 1 0 0 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 1 1 1 0 0 0 1 1 1 0 1 0 0 1 1 1 1 1 0 0

===== Repeated Time Measurement =====
== Greedy (repeat 1000 times) ==
Total Time: 92.000 ms
Average Time: 0.0920 ms

== Single Flip (repeat 1000 times) ==
Total Time: 4426.000 ms
Average Time: 4.4260 ms

== Big Enough (esp=0.100000) (repeat 1000 times) ==
Total Time: 1482.000 ms
Average Time: 1.4820 ms

== Kernighan-Lin (repeat 1000 times) ==
Total Time: 8422.000 ms
Average Time: 8.4220 ms
```