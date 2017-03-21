## Mushroom: Multi-Progress/Multi-Thread B<sup>link</sup> Tree Index
[中文版 README](./README.md)


### Feature
+ Concurrent B<sup>link</sup> Tree Index
+ Prefix Compression (Lazy)
+ Latch Manager
+ Mulit-Thread (Thread Pool, Bounded Thread-Safe Queue)

******

### B<sup>link</sup> Tree Benchfuck
`key length: 16 bytes`  
`total key: 10 million`  
`total size: 160 M`  
`CPU: Intel i3  2.1GHz  4 threads`

| Version | Multi-Thread | Multi-Progress | Sorting Time(s) |       Improvements       |
|:------:|:-------:|:---------:|:-----------:|:--------------------------:|
| 0.1.0  |  No   |  No  |16.00    ||
| 0.2.0  |  Yes  |  No  |12.32    |   two-phase lock based concurrent index   |
| 0.2.1  |  Yes  |  No  |11.28    |         latch manager optimization         |
| 0.3.0  |  Yes  |  No  |10.94    |  prefix compaction, reducing B<sup>link</sup> tree memory about 9.1 % |
| 0.4.0  |  Yes  |  No  |11.44    |  second-time mapping mulit-thread queue, reducing total program memory up to 50 %|
| 0.4.1  |  Yes  |  No  |\    | merge latch manager and page manager，reduce 1 lock per operation |
| 0.4.2  |  Yes  |  No  |\    | change the way B link tree root split |
| 0.4.3  |  Yes  |  No  |\   |  new test strategy, threads conduct operations without going through the queue |
| 0.4.4  |  Yes  |  No  |\   |  latch manager refactoring |
| 0.5.0  |  Yes  |  No  |11.70 / 9.00   | fix **BUG** that exists from 0.4.1 to 0.4.4 |
