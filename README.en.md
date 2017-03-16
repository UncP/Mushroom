##Mushroom: NoSQL Database
[中文版 README](./README.md)

###Mushroom: Multi-Thread NoSQL Database based on B link Tree Index

####This is an upgrade of [Up Database](http://www.github.com/UncP/Up_Database) and [pear Database](http://www.github.com/UncP/pear)


###B link Tree Performance
`key length: 16 bytes`  
`key tuples: 10 million`

|  Version  |  Thread  |  Sorting Time(s) |           Improvements            |
|--------|:-------:|:-----------:|:--------------------------:|
| 0.1.0  |  Single |    16.00    ||
| 0.2.0  |  Multi  |    12.32    |   two-phase lock based concurrent index   |
| 0.2.1  |  Multi  |    11.28    |         latch manager optimization         |
| 0.3.0  |  Multi  |    10.94    |  prefix compaction, reducing index memory about 9.1 % |
| 0.4.0  |  Multi  |    11.44    |  second-time mapping mulit-thread queue, reducing total program memory up to 50 %|
| 0.4.1  |  Multi  |    \    | Merge Latch Manager and Page Manager，Reduce 1 lock per insertion by memory hacking |
| 0.4.2  |  Multi  |    \    | Change the way B link Tree root split |
| 0.4.3  |  Multi  |    \   |  Threads conduct operations without going through the queue |


###TODO
- [x] B link Tree Index Engine
- [x] Thread Pool
- [x] Thread Safe Queue(Bounded)
- [x] Latch Manager
- [x] Concurrent Index
- [x] Prefix Compression(Lazy)
- [ ] Data Manager
