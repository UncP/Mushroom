##Mushroom: NoSQL Database
[中文版 README](./README.md)

###Mushroom: NoSQL Database based on B link Tree Index

####This is an upgrade of [Up Database](http://www.github.com/UncP/Up_Database)and[pear Database](http://www.github.com/UncP/pear)


###Feature
- More Simplified Back-End
- Support Longer Data (nearly 4 k-bytes)
- More Execllent Insertoin Speed


###B link Tree Performance
`key length: 16 bytes`
`key tuples: 10 million`

- Version 0.1.0 single thread sorting time : 16 s

- Version 0.2.0 multi thread sorting time :  12.32 s

- Version 0.2.1 multi thread sorting time :  11.28 s
	+ latch manager optimization


###TODO
- [x] B link Tree Index Engine
- [x] Thread Pool
- [x] Thread Safe Queue(Unbounded, Bounded)
- [x] Latch Manager
- [x] Concurrent Index
- [ ] Data Manager
