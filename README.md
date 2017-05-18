## Mushroom（蘑菇）：分布式内存索引
[English Version of README](./README.en.md)

[![Author](https://img.shields.io/badge/Author-UncP-brightgreen.svg)](https://github.com/UncP)
[![Version](https://img.shields.io/badge/Version-0.8.0-blue.svg)]()
[![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](https://travis-ci.org/UncP/Mushroom)
[![License](https://img.shields.io/badge/License-BSD-red.svg)](./LICENSE)

### Behold, the power of Mushroom!

### Feature
+ 并发B<sup>link</sup>树索引（前缀压缩，二次哈希页面管理器、线程池、映射队列，锁管理器）
+ TCP通信库
+ RPC框架
+ 分布式一致性算法——Raft

******

### B<sup>link</sup> Tree BenchFuck
|关键值数量|关键值长度|     CPU    |    内存   | 版本号 | 排序时间（秒）|        备注      |
|:-------:|:--------:|:----------:|:---------:|:------:|:-----------:|:------------------:|
| 1 亿条 |  16 字节 | Intel i3 2.1 GHz 4 核 |4 G| 0.6.1 | 142.5 | 占用内存约2.7G |


### 版本信息
| 版本 |           备注             |
|:------:|:---------------------------:|
| 0.1.0  | |
| 0.2.0  |         读写锁并发索引          |
| 0.2.1  |         锁管理器优化            |
| 0.3.0  | 引入前缀压缩，B<sup>link</sup>树占用内存减少约 9.1 %|
| 0.4.0  | 二次映射多线程工作队列，减少程序使用内存超过 50 %|
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
| 0.8.0  |       TCP通信库和RPC框架    |
| 0.9.0  |       Raft（实现中）   |

### 其他
+ 版本0.6.0是第一个稳定版本
+ 你可以在这个[知乎专栏](https://zhuanlan.zhihu.com/b-tree)里找到**Mushroom**的介绍
