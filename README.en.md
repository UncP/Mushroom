## Mushroom: Concurrent B<sup>link</sup> Tree Index
[中文版 README](./README.md)

[![Author](https://img.shields.io/badge/Author-UncP-brightgreen.svg)](./LICENSE)
[![Version](https://img.shields.io/badge/Version-0.7.0-blue.svg)]()
[![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](https://travis-ci.org/UncP/Mushroom)
[![License](https://img.shields.io/badge/License-BSD-red.svg)](./LICENSE)

### Behold, the power of Mushroom!

### Feature
+ Concurrent B<sup>link</sup> Tree Index (Prefix Compression)
+ Log-Structured Merge Tree
+ Two-Phase Hashing Page Manager
+ Mulit-Thread (Latch Manager, Thread Pool, Bounded Thread-Safe Queue)
+ Multi-Process (Shared Memory Mapping)

******

### Version Information
| Version |       Improvements       |
|:------:|:--------------------------:|
| 0.1.0  | |
| 0.2.0  |   two-phase lock based concurrent index   |
| 0.2.1  |         latch manager optimization         |
| 0.3.0  | implement  prefix compaction, reducing B<sup>link</sup> tree memory about 9.1 % |
| 0.4.0  |  second-time mapping mulit-thread queue, reducing total program memory up to 50 %|
| 0.4.1  | merge latch manager and page manager，reduce 1 lock per operation |
| 0.4.2  |            change the way B link tree root split |
| 0.4.3  |  new test strategy, threads conduct operations without going through the queue |
| 0.4.4  |               latch manager refactoring     |
| 0.5.0  | fix **BUG**(atomic operation bug) that exists from 0.4.1 to 0.4.4 |
| 0.6.0  | multi-process supported, fix B<sup>link</sup> tree search **bug**|
| 0.6.1  | two-phase hashing page manager, implement lazy page allocation|
| 0.6.2  | reduce dependency on standard library, speed up compile, reduce program size about 42.1%|
| 0.6.4  |using posix spin lock, Optimize MushroomDB and BLinkTree structure|
| 0.7.0  |         Log-Structured Merge Tree |
| 0.8.0  |       TCP Communication Library (in construction)   |


### B<sup>link</sup> Tree BenchFuck
|total key|key length| total size |     CPU    | Mem |
|:-------:|:--------:|:---------:|:--------------:|:----:|
|100 million | 16 bytes | 1.6 G | Intel i3 2.1 GHz 4 cores|4 G|

| Version | Sorting Time(s) |       Detail       |
|:------:|:------:|:-----------------------------------:|
| 0.6.1 | 142.5 | B<sup>link</sup> tree occupies memory about 2.7G |

### Other
+ Version 0.6.0 is the first stable version
