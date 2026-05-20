## 实验文档模块对应代码关系

### 1. Disk Manager / Buffer Pool Manager
对应实验：`#1 DISK AND BUFFER POOL MANAGER`

- 代码文件
  - bitmap_page.h
  - bitmap_page.cpp
  - disk_manager.h
  - disk_manager.cpp
  - lru_replacer.h
  - lru_replacer.cpp
  - buffer_pool_manager.h
  - buffer_pool_manager.cpp

- 要做什么
  - 实现位图页（Bitmap Page）：分配/回收磁盘中页位标记
  - 实现磁盘页管理：逻辑页号与物理页号映射，分配/释放页
  - 实现缓冲池替换：LRU 替换算法，Pin/Unpin 页
  - 实现缓冲池管理：FetchPage、UnpinPage、NewPage、FlushPage 等页面调度和脏页写盘逻辑

---

### 2. Record Manager
对应实验：`#2 RECORD MANAGER`

- 代码文件
  - row.h
  - row.cpp
  - schema.h
  - schema.cpp
  - column.h
  - column.cpp
  - table_heap.h
  - table_heap.cpp
  - table_iterator.h
  - table_iterator.cpp

- 要做什么
  - 实现 `Row`、`Schema`、`Column` 的序列化 / 反序列化 / 大小计算
  - 实现 `TableHeap` 堆表：插入、删除、更新、查找记录
  - 实现 `TableIterator`：遍历表中所有记录
  - 支持 slotted-page 结构、逻辑删除、记录定位

---

### 3. Index Manager
对应实验：`#3 INDEX MANAGER`

- 代码文件
  - b_plus_tree_page.h
  - b_plus_tree_page.cpp
  - b_plus_tree_internal_page.h
  - b_plus_tree_internal_page.cpp
  - b_plus_tree_leaf_page.h
  - b_plus_tree_leaf_page.cpp
  - b_plus_tree.h
  - b_plus_tree.cpp
  - index_iterator.h
  - index_iterator.cpp

- 要做什么
  - 实现 B+ 树页结构：内部页和叶子页的插入/删除/分裂/合并/借用
  - 实现 B+ 树主逻辑：Create、Insert、Delete、Search、Root 更新
  - 实现索引迭代器：顺序遍历叶子节点，用于范围查询

---

### 4. Catalog Manager
对应实验：`#4 CATALOG MANAGER`

- 代码文件
  - catalog.h
  - catalog.cpp
  - indexes.h
  - indexes.cpp
  - table.h
  - table.cpp

- 要做什么
  - 管理表、列、索引的元信息
  - 实现元数据序列化 / 反序列化
  - 实现 `CatalogManager`：创建/删除表、创建/删除索引、查询表和索引信息
  - 提供上层调用接口，供执行器和 planner 查询表结构、索引结构

---

### 5. Planner / Executor
对应实验：`#5 PLANNER AND EXECUTOR`

- 代码文件
  - main.cpp
  - execute_engine.h
  - execute_engine.cpp
  - abstract_executor.h
  - delete_executor.h
  - insert_executor.h
  - seq_scan_executor.h
  - index_scan_executor.h
  - update_executor.h
  - 以及对应的 `src/executor/...` 实现文件

- 要做什么
  - Parser已给出，主要实现执行器部分
  - 实现 5 个算子：SeqScan、IndexScan、Insert、Update、Delete
  - 实现 `ExecuteEngine` 中对 SQL 命令的调度：建库、建表、建索引、查询、删除、显示等
  - 让 SQL 语法树经过 Planner/Executor 正常执行

> 注意：实验文档里 Planner 部分大多数代码已经给出，重点是执行算子和 `ExecuteEngine` 的接口。

---

### 6. Recovery Manager
对应实验：`#6 RECOVERY MANAGER`

- 代码文件
  - log_rec.h
  - recovery_manager.h

- 要做什么
  - 定义内存日志结构 `LogRec`
  - 实现 `RecoveryManager` 初始化、RedoPhase、UndoPhase
  - 实现各种日志生成函数：插入、删除、更新、事务开始/提交/回滚
  - 用内存结构模拟简单恢复流程

---

### 7. Lock Manager
对应实验：`#7 LOCK MANAGER`

- 代码文件
  - lock_manager.h
  - lock_manager.cpp

- 要做什么
  - 实现锁请求队列、锁升级、共享/排他锁授予与释放
  - 按事务隔离级别判断是否允许加锁
  - 这部分属于 bonus，实验文档中也说明是独立的并发控制模块

---

## 额外说明

- `Parser` 模块的词法语法已经给出，通常不用你实现；
- `Planner` 部分主要是将语法树转成计划树，实验文档说“本模块中你不用实现任何代码”；
- 当前仓库里实际路径跟实验文档描述基本一致，但 index 和 index 等是你实际要看和改的地方。
### 说明

> 以上文件中都有“TODO: Student Implement”标记，说明这些部分仍需完成实现。

