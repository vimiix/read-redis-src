# 阅读redis源码

> **基于redis 3.2.5**

通读 **redis** 源码记录，直接在文件源码中附加中文注释。

源码下载：

```bash
wget http://download.redis.io/releases/redis-3.2.5.tar.gz
tar zxvf redis-3.2.5.tar.gz
```

## 一般阅读顺序：

### 第一阶段

熟悉内存管理和数据结构的实现

- 内存分配 zmalloc.c和zmalloc.h
- 动态字符串 sds.h和sds.c
- 双端链表 adlist.c和adlist.h
- 字典 dict.h和dict.c
- 跳跃表 server.h文件里面关于zskiplist结构和zskiplistNode结构，以及t_zset.c中所有zsl开头的函数，比如 zslCreate、zslInsert、zslDeleteNode等等。
- 基数统计 hyperloglog.c 中的 hllhdr 结构， 以及所有以 hll 开头的函数

### 第二阶段

熟悉Redis的内存编码结构的实现

- 整数集合数据结构 intset.h和intset.c
- 压缩列表数据结构 ziplist.h和ziplist.c

### 第三阶段

熟悉Redis数据类型的实现

- 对象系统 object.c
- 字符串键 t_string.c
- 列表建 t_list.c
- 散列键 t_hash.c
- 集合键 t_set.c
- 有序集合键 t_zset.c中除 zsl 开头的函数之外的所有函数
- HyperLogLog键 hyperloglog.c中所有以pf开头的函数

### 第四阶段

熟悉Redis数据库的实现

- 数据库实现 redis.h文件中的redisDb结构，以及db.c文件
- 通知功能 notify.c
- RDB持久化 rdb.c
- AOF持久化 aof.c

以及一些独立功能模块的实现

- 发布和订阅 redis.h文件的pubsubPattern结构，以及pubsub.c文件
事务 redis.h文件的multiState结构以及multiCmd结构，multi.c文件

### 第五阶段

熟悉客户端和服务器端的代码实现

- 事件处理模块 ae.c/ae_epoll.c/ae_evport.c/ae_kqueue.c/ae_select.c
- 网路链接库 anet.c和networking.c
- 服务器端 server.c
- 客户端 redis-cli.c

独立功能模块

- lua脚本 scripting.c
- 慢查询 slowlog.c
- 监视 monitor.c

### 第六阶段

熟悉Redis多机部分的代码实现

- 复制功能 replication.c
- Redis Sentinel sentinel.c
- 集群 cluster.c

### 其他代码文件介绍

关于测试方面的文件有：

- memtest.c 内存检测
- redis_benchmark.c 用于redis性能测试的实现。
- redis_check_aof.c 用于更新日志检查的实现。
- redis_check_dump.c 用于本地数据库检查的实现。
- testhelp.c 一个C风格的小型测试框架。

一些工具类的文件如下：

- bitops.c GETBIT、SETBIT 等二进制位操作命令的实现
- debug.c 用于调试时使用
- endianconv.c 高低位转换，不同系统，高低位顺序不同
- help.h 辅助于命令的提示信息
- lzf_c.c 压缩算法系列
- lzf_d.c 压缩算法系列
- rand.c 用于产生随机数
- release.c 用于发布时使用
- sha1.c sha加密算法的实现
- util.c 通用工具方法
- crc64.c 循环冗余校验
- sort.c SORT命令的实现

一些封装类的代码实现：

- bio.c background I/O的意思，开启后台线程用的
- latency.c 延迟类
- migrate.c 命令迁移类，包括命令的还原迁移等
- pqsort.c 排序算法类
- rio.c redis定义的一个I/O类
- syncio.c 用于同步Socket和文件I/O操作

### 参考链接：

[1] [http://zcheng.ren/categories/Redis/](http://zcheng.ren/categories/Redis/)