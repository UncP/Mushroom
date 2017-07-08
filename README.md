## Mushroom（蘑菇）：分布式内存索引
[English Version of README](./README.en.md)

[![Author](https://img.shields.io/badge/Author-UncP-brightgreen.svg)](https://github.com/UncP)
[![Version](https://img.shields.io/badge/Version-1.1.1-blue.svg)]()
[![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](https://travis-ci.org/UncP/Mushroom)
[![License](https://img.shields.io/badge/License-BSD-red.svg)](./LICENSE)

#### Mushroom是一个C++11编写、不依赖第三方库的轻量级Linux环境分布式内存索引，它由并发B<sup>link</sup>树索引，TCP通信库，RPC框架，Raft一致性算法组成。

### Behold, the power of Mushroom!

### 尝试
`首先运行脚本gen_test_data.sh生成测试数据，然后进入src目录`<br>
`./run index 100，单机测试分布式索引，100是索引数量（因为Raft没有深入优化过，所以请将数量控制在1-1000）`<br>
`./run blink queue 100，测试多线程索引（通过队列），数量可以是1到1000万`<br>
`./run blink thread 100，测试多线程索引，数量可以是1到250万（实际乘以4）`<br>
`./run art thread 100，测试Adaptive Radix Tree，数量可以是1到250万（实际乘以4）`<br>


### 版本信息
| 版本 |           备注             |
|:------:|:---------------------------:|
| 0.1.0  | |
| 0.2.0  |         读写锁并发索引          |
| 0.2.1  |         锁管理器优化            |
| 0.3.0  | 引入前缀压缩，B<sup>link</sup>树占用内存减少约 9.1 %|
| 0.4.0  | 实现映射队列，减少程序使用内存超过 50 %|
| 0.4.1  | 合并锁管理器与页面管理器，使每次操作减少1把锁|
| 0.4.2  |       修改根节点分裂方式    |
| 0.4.3  | 增加测试策略，多线程不经过队列直接进行任务|
| 0.4.4  |           重构锁管理器      |
| 0.5.0  |      修复从版本0.4.1到0.4.4一直存在的**bug**（原子操作bug）|
| 0.6.0  | 共享内存映射支持多进程，修复搜索**bug**，正确实现并发B<sup>link</sup>树|
| 0.6.1  |   二次哈希页面管理器，实现页面的懒惰分配|
| 0.6.2  | 减少对标准库的依赖，加快编译速度，减少程序体积约42.1%|
| 0.6.4  | 使用posix自旋锁，优化MushroomDB和BLinkTree结构  |
| 0.7.0  |        日志结构合并树（LSM Tree）  |
| 0.8.0  |     TCP通信库、RPC框架    |
| 0.9.0  |        Raft     |
| 0.9.1  |     ACID (批操作)、修改加锁策略      |
| 0.9.2  |   优化Raft状态变化，提高Liveness      |
| 1.0.0  |    分布式内存索引    |
| 1.1.0  |    B<sup>link</sup>树结点从1/2满提高到2/3满（不可行）    |
| 1.1.1  |     优化Raft内存使用（牺牲可读性）   |
| 1.2.0  |     Adaptive Radix Tree |


### 其他
+ 你可以在这个[知乎专栏](https://zhuanlan.zhihu.com/b-tree)里找到**Mushroom**的介绍
+ master分支是开发分支，编译不一定能通过
+ 有问题请开issue，我会在24小时内回复
