完整的梳理。
![[Pasted image 20260515153936.png]]

这就是整个项目的分层架构。下面按面试考点梳理你应该掌握的技术细节：

---
**数据库层 — 表设计**

三张表的主键和约束你要能默写出来。`book` 表有个五字段联合唯一约束 `(category, press, author, title, publish_year)`，意思是"同一本书"不能重复入库。`borrow` 表主键是 `(card_id, book_id, borrow_time)` 三字段联合，因为同一个人可以对同一本书多次借还。`return_time=0` 表示未归还。外键设了 `ON DELETE CASCADE`，删卡会级联删借阅记录。

**业务逻辑层 — 你写的 12 个方法**

每个方法的模式都是：拿 Connection → PreparedStatement 设参数 → 执行 → 成功 commit / 异常 rollback。面试不会问每个方法的细节，但你要能说清三个重点：

第一，**PreparedStatement 防 SQL 注入**。所有用户输入用 `?` 占位，不拼字符串。面试追问原理：预编译阶段 SQL 结构已固定，参数只作为数据传入，不会被解析为 SQL 关键字。

第二，**批量入库用 `addBatch()` + `executeBatch()`**。接口注释里明确要求不能循环调用单条 `storeBook`。好处是减少网络往返（一次发所有 SQL 给数据库），而且如果其中一条失败，整个批次 rollback，保证原子性。

第三，**`borrowBook` 里的 `FOR UPDATE` 并发控制**。这是最核心的考点。InnoDB 在 RR 隔离级别下，普通 SELECT 是快照读（MVCC），两个事务各自读到同一个 stock 值，都以为有库存，就会超卖。`SELECT ... FOR UPDATE` 做了两件事：强制当前读（读最新已提交值）+ 对该行加排他锁（X 锁），其他事务的 FOR UPDATE 会阻塞等待直到锁释放。你要能说出"快照读 vs 当前读"这组对比。

**HTTP 服务层 — Main.java**

用 JDK 内置的 `com.sun.net.httpserver.HttpServer` 监听 8000 端口，不依赖 Spring。三个 Handler 按 RESTful 风格设计：GET 查询、POST 创建、PUT 修改、DELETE 删除。`BookHandler` 额外有四个子路由 `/book/borrow`、`/book/return`、`/book/stock`、`/book/batch`。没用 JSON 库，手写了字符串解析方法。`addCorsHeaders` 处理跨域，因为前端 dev server 和后端不同端口，浏览器会发 OPTIONS 预检请求。

**前端层 — Vue 3 + Element Plus**

`Card.vue` 你补全了 `ConfirmRemoveCard`：`axios.delete` 发请求，成功刷新列表，失败提示"有未归还图书"。`Book.vue` 从零写，功能最全：8 个查询条件拼 query string、单本入库、批量入库（前端解析 CSV 文本 → JSON 数组 → POST 给后端）、编辑 PUT、删除 DELETE 带确认弹窗、库存增减、借书还书。所有操作后都调 `QueryBooks()` 刷新表格。

**事务管理 — 贯穿全局的设计**

`DatabaseConnector` 里 `conn.setAutoCommit(false)`，意味着每条 SQL 不会自动提交，必须手动 `commit()` 或 `rollback()`。你的每个方法都用 try-catch 包裹，成功走 commit，任何异常走 rollback。这保证了每个 API 调用是一个完整事务。

**并发测试 — BorrowThread.java**

测试代码用 16 个线程同时借同一本书，用 `Semaphore` 让所有线程先阻塞、然后同时放行。只有 stock 本能成功，`successOps` 的最终值应该等于初始库存。这就是验证你的 `FOR UPDATE` 是否正确工作的。
