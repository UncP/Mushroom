## Mushroom: Distributed In-Memory Index
[中文版 README](./README.md)

[![Author](https://img.shields.io/badge/Author-UncP-brightgreen.svg)](./LICENSE)
[![Version](https://img.shields.io/badge/Version-1.1.1-blue.svg)]()
[![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](https://travis-ci.org/UncP/Mushroom)
[![License](https://img.shields.io/badge/License-BSD-red.svg)](./LICENSE)

### Mushroom is a Linux light-weight distributed in-memory index written in C++11, consists of concurrent B<sup>link</sup> tree, TCP communication library, RPC framework and Raft consensus algorithm.

### Behold, the power of Mushroom!

### B<sup>link</sup> Tree BenchFuck
|total key|key length| total size |     CPU    | Mem |
|:-------:|:--------:|:---------:|:--------------:|:----:|
|100 million | 16 bytes | 1.6 G | Intel i3 2.1 GHz 4 cores|4 G|

| Version | Sorting Time(s) |       Detail       |
|:------:|:------:|:-----------------------------------:|
| 0.6.1 | 142.5 | B<sup>link</sup> tree occupies memory about 2.7G |

### Version Information
| Version |       Improvements       |
|:------:|:--------------------------:|
| 0.1.0  | |
| 0.2.0  |   two-phase lock based concurrent index   |
| 0.2.1  |         latch manager optimization         |
| 0.3.0  | implement  prefix compaction, reducing B<sup>link</sup> tree memory about 9.1 % |
| 0.4.0  |  implement **Mapping Queue**, reducing total program memory up to 50 %|
| 0.4.1  | merge latch manager and page manager，reduce 1 lock per operation |
| 0.4.2  |            change the way B link tree root split |
| 0.4.3  |  new test strategy, threads conduct operations without going through the queue |
| 0.4.4  |               latch manager refactoring     |
| 0.5.0  | fix **BUG**(atomic operation bug) that exists from 0.4.1 to 0.4.4 |
| 0.6.0  | multi-process supported, fix B<sup>link</sup> tree search **bug**|
| 0.6.1  | two-phase hashing page manager, implement lazy page allocation|
| 0.6.2  | reduce dependency on standard library, speed up compile, reduce program size about 42.1%|
| 0.6.4  |using posix spin lock, Optimize MushroomDB and BLinkTree structure|
| 0.7.0  |      Log-Structured Merge Tree    |
| 0.8.0  |   TCP Communication Library & RPC Framework    |
| 0.9.0  |        Raft       |
| 0.9.1  |   ACID (batch operation) & modify locking strategy   |
| 0.9.2  |   optimize Raft state transfer, improve liveness   |
| 1.0.0  |     distributed in-memory index   |
| 1.1.0  |    B<sup>link</sup> tree node occupies 2/3 of node's space, originally 1/2 (Aborted) |
| 1.1.1  |     optimize raft memory use   |


### Try Mushroom
`first run ./gen_test_data.sh to generate test data, then enter src directory`<br>
`./run index 100, test distributed index on single machine，100 represents index number(raft is not well optimized, please limit number to 1-10000)`<br>
`./run mushroom thread 100, test multi-thread b link tree, number can be 1 to 10 million`<br>
`./run mushroom queue 100, test multi-thread b link tree, number can be 1 to 10 million`<br>


### Other
+ Version 0.6.0 is the first stable version
