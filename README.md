##Mushroom 非关系型数据库
[English Version of README](./README.en.md)

###蘑菇：基于 B link 树索引的非关系型数据库

####这是[Up Database](http://www.github.com/UncP/Up_Database)以及[pear Database](http://www.github.com/UncP/pear)的升级版本


###特点
- 更加简洁的后端模型
- 支持更大的数据长度（接近4k字节）
- 更加优秀的插入性能


###B link 树性能信息
`关键值长度： 16 字节`
`关键值数量： 1000 万组`

- 版本 0.1.0 排序用时 16 秒


###TODO
- [x] B link 树索引管理器
- [x] 线程池
- [x] 多线程工作队列(无界，有界)
- [x] 锁管理器
- [ ] 数据管理器
