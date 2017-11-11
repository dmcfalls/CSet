# CSet

Author: Dan McFalls (dmcfalls@stanford.edu)

An implementation of the <code>set</code> data structure in C using an ordered, variable-size array and <code>void*</code> interface.

A <code>set</code> is a collection of distinct objects. The CSet has functionality for <code>add</code>, <code>contains</code>, and <code>remove</code>, as well as basic set operations including <code>cardinality</code>, <code>isSubsetOf</code>, <code>union</code>, <code>intersect</code>, <code>difference</code>, and <code>powerSet</code>.

Maintains the internal storage as a sorted array. Uses a client comparator function to compare elements. Provides necessary components for nesting sets within sets, which makes an operation like <code>powerSet</code> possible.

The <code>add</code>, <code>contains</code>, and <code>remove</code> functions use a binary searching algorithm to access the correct index of the array so that each performs in O(log n) time where n is the cardinality of the set. The <code>powerSet</code> method is implemented using bitvectors from 0 to n^2 as a way to exhaust every possible combination of set's elements and generate the power set in O(n^3) time (with n as the set's cardinality).
