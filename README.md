# CSet

Author: Dan McFalls (dmcfalls@stanford.edu)

An implementation of the <code>set</code> data structure in C using an ordered array and <code>void*</code> interface.

A <code>set</code> is a collection of distinct objects. The CSet has functionality for <code>add</code>, <code>contains</code>, and <code>remove</code>, as well as basic set operations including <code>isSubsetOf</code>, <code>union</code>, <code>intersect</code>, <code>difference</code>, and <code>power set</code>.

Maintains the internal storage as a sorted array. Uses a client comparator function to compare elements. Provides necessary components for nesting sets within sets, which makes an operation like <code>power set</code> possible.
