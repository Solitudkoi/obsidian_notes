# 模块七 Lock Manager —— 验收讲解（基于你的真实代码）

> 文件：`src/concurrency/lock_manager.{h,cpp}`，测试：`test/concurrency/lock_manager_test.cpp`（10 条）。
> 下面每一节都对着你仓库里**已有的代码**讲。验收按「功能 → 代码逻辑 → 怎么测」三块过。

---

## 〇、一句话总览

Lock Manager 给并发事务在**行（RowId）粒度**上发放/释放**共享锁 S** 和**排他锁 X**，遵循**两阶段封锁 2PL**，支持 **S→X 锁升级**，并用一个后台线程基于**等待图（waits-for graph）**做**死锁检测**——发现环就中止环里**最年轻**的事务来打破死锁。

---

## 一、实现的功能（对应哪些接口）

| 功能 | 接口 | 说明 |
|---|---|---|
| 申请共享锁 | `LockShared` | 读操作加 S 锁；多个事务的 S 锁可共存 |
| 申请排他锁 | `LockExclusive` | 写操作加 X 锁；与任何其它锁互斥 |
| 锁升级 | `LockUpgrade` | 持有 S 锁的事务把它升级成 X 锁 |
| 释放锁 | `Unlock` | 放锁，并推进 2PL 阶段 |
| 等待图维护 | `AddEdge / RemoveEdge / GetEdgeList` | 增删边、导出全部边（测试用） |
| 环检测 | `HasCycle` | DFS 找环，返回环中最年轻事务 |
| 后台死锁检测 | `RunCycleDetection` | 周期性重建等待图、找环、中止牺牲品 |

**两个隔离级别相关的规则**（你代码里体现了）：
- `READ_UNCOMMITTED` 不允许加 S 锁（读不加锁）。
- `READ_COMMITTED` 下 S 锁可即时释放、**不触发**进入收缩阶段。

---

## 二、核心数据结构（`lock_manager.h`）

讲代码前先把"零件"说清楚，否则讲函数时听者会懵。

**`LockRequest`**（一条锁请求）：`txn_id_`（谁请求）、`lock_mode_`（想要 S 还是 X）、`granted_`（已经授予的模式，`kNone` 表示还在排队等待）。

**`LockRequestQueue`**（每个 RowId 一个，挂在该行上的所有请求）：
- `req_list_`：`std::list<LockRequest>`，请求队列。
- `req_list_iter_map_`：`txn_id → 该请求在 list 里的迭代器`，让"按事务号找/删请求"是 O(1)。
- `cv_`：条件变量，阻塞/唤醒等这把锁的线程。
- `is_writing_`：这行上是否有人持 X 锁。
- `sharing_cnt_`：这行上有几个事务持 S 锁。
- `is_upgrading_`：这行上是否有一个升级正在进行（防止两个升级同时来）。

**`LockManager` 私有成员**：
- `lock_table_`：`RowId → LockRequestQueue`，全局锁表。
- `latch_`：一把大互斥锁，保护整个锁表和等待图（所有公开函数进来先 `lock(latch_)`）。
- `waits_for_`：`txn → set<txn>`，等待图（我等谁）。用 `std::set` 让邻居天然升序。
- `txn_mgr_`：事务管理器指针，用来按 txn_id 拿到 `Txn*`。

> **可能被问**：为什么用一把大 latch 而不是每行一把？答：实现简单、正确性容易保证；行级数据库通常每个队列一把锁更细，但本实验规模下单 latch 足够，且死锁检测要扫全表更省心。

---

## 三、代码逻辑（逐函数，对着 `lock_manager.cpp`）

### 3.1 两个辅助函数

**`LockPrepare`（L16）**：加锁公共前置。①若事务已在 `kShrinking`（收缩阶段）还来加锁 → 违反 2PL，把它设 `kAborted` 并抛 `kLockOnShrinking`；②`lock_table_[rid];` 这一句是**故意触发 `operator[]`** 在锁表里就地默认构造一个队列——因为 `LockRequestQueue` 含 `condition_variable`，不可拷贝/移动，必须原地构造。

**`CheckAbort`（L28）**：线程被 `cv_` 唤醒后调用。若发现自己已被死锁检测设成 `kAborted`，就把自己的请求从队列删掉并抛 `kDeadlock`。**这是"被选为牺牲品的线程"抛出死锁异常的地方。**

### 3.2 `LockShared`（L38）—— 申请共享锁

逻辑顺序：
1. `lock(latch_)` 上大锁。
2. 若隔离级别是 `READ_UNCOMMITTED` → 设 Aborted、抛 `kLockSharedOnReadUncommitted`（读未提交不加 S 锁）。
3. `LockPrepare`（收缩阶段检查 + 建队列）。
4. `EmplaceLockRequest(txn, kShared)` 把请求挂到队列。
5. **等待条件**：`if (q.is_writing_)` —— 有人持 X 锁就在 `cv_` 上等，谓词是 `被Abort || !is_writing_`（写锁放了，或自己被判死锁）。
6. `CheckAbort`：醒来后若被判死锁就抛。
7. 授予：加入事务的 `SharedLockSet`、`sharing_cnt_++`、把该请求 `granted_ = kShared`。

> 关键点：**S 锁只和 X 锁冲突**，所以只判 `is_writing_`，不判 `sharing_cnt_`（多个 S 可共存）。

### 3.3 `LockExclusive`（L64）—— 申请排他锁

和 S 几乎一样，差别只在**等待条件更严**：
- `if (q.is_writing_ || q.sharing_cnt_ > 0)` —— 有 X 锁**或**有任何 S 锁都要等。
- 谓词：`被Abort || (!is_writing_ && sharing_cnt_ == 0)`（这行上彻底没人了）。
- 授予：加入 `ExclusiveLockSet`、`is_writing_ = true`、`granted_ = kExclusive`。

> 关键点：**X 锁和一切锁互斥**，所以要等到既没写锁、也没读锁。

### 3.4 `LockUpgrade`（L87）—— S 升级为 X（**重点，容易被追问**）

为什么不能简单"放 S 再加 X"？因为中间会有缝隙让别的事务插进来。升级要**原子地**把自己的 S 变 X：
1. **最先**做收缩阶段检查（抛 `kLockOnShrinking`）。**注意必须最先**——因为升级时队列里可能已经没有这条请求了，先查状态最稳妥。
2. `if (q.is_upgrading_)` 已有别的事务在升级 → 抛 `kUpgradeConflict`（同一行同时只允许一个升级）。
3. 把自己原来的 S 请求改成"**待授予的 X 请求**"：`lock_mode_=kExclusive`、`granted_=kNone`、`sharing_cnt_--`（让出自己那份 S 计数）、`is_upgrading_=true`。
4. **等待**：`if (is_writing_ || sharing_cnt_>0)` 等到这行上只剩自己（谓词同 X 锁）。
5. 等待中若被判死锁牺牲品：`is_upgrading_=false` 后 `CheckAbort` 抛 `kDeadlock`。
6. 成功：`is_upgrading_=false`、`is_writing_=true`、`granted_=kExclusive`，并把 rid 从 `SharedLockSet` 挪到 `ExclusiveLockSet`。

### 3.5 `Unlock`（L130）—— 释放锁 + 推进 2PL

1. 从事务的 `SharedLockSet / ExclusiveLockSet` 里擦掉 rid。
2. 在队列里找这条请求，读出它的 `granted_`（是 S 还是 X）。
3. **2PL 阶段推进**：`if 事务在 GROWING 且 !(READ_COMMITTED 且 granted 是 S)` → 设 `kShrinking`。即：第一次放锁就进入收缩阶段；但 READ_COMMITTED 放 S 锁是例外，不收缩（这样它还能继续加锁）。
4. 维护计数：S 锁 `sharing_cnt_--`，X 锁 `is_writing_=false`。
5. `EraseLockRequest` 删请求，`cv_.notify_all()` 唤醒这行上所有等待者（让它们重新检查谓词、有机会拿锁）。

### 3.6 等待图与环检测

**`AddEdge`（L164）**：`waits_for_[t1].insert(t2)`（t1 等 t2）。
**`RemoveEdge`（L169）**：删边，若 t1 没有出边了就把 t1 整个擦掉（保持图干净）。
**`GetEdgeList`（L307）**：把 `waits_for_` 摊平成 `(from,to)` 列表，**测试用**。

**`HasCycle`（L183）—— DFS 找环（重点）**：
1. 收集所有结点放进 `std::set nodes`（升序）。
2. 标准 DFS，维护 `visited`（全局访问过）、`path`（当前 DFS 路径，按顺序）、`on_path`（路径上的点，O(1) 判回边）。
3. 遍历邻居 `for (v : waits_for_[u])`——因为是 `std::set`，**邻居升序**。
4. 若 `on_path.count(v)`：找到**回边 u→v**，说明有环。环就是 `path` 里从 v 到末尾那一段，遍历这段取**最大 txn_id**（= 最年轻），写入 `newest_tid_in_cycle` 返回 true。
5. 起点也按 `nodes` 升序遍历。**升序起点 + 升序邻居**保证：找到的是确定的"第一个环"，且和参考测试期望一致。

> **为什么取最年轻（txn_id 最大）？** 最年轻=最晚开始的事务，回滚它浪费的工作最少；固定策略也避免反复互杀。

**`RunCycleDetection`（L255）—— 后台死锁检测线程（最重点）**：
循环（`while enable_cycle_detection_`）：
1. `sleep(interval)` 睡一个检测周期。
2. 上 `latch_`，**清空并重建等待图**：遍历 `lock_table_` 每个队列，把请求分成 `holders`（`granted_ != kNone`，已持锁）和 `waiters`（`granted_ == kNone`，在等待）；对每个 waiter 向每个 holder `AddEdge(waiter, holder)`。同时用 `waiting_on[waiter]=rid` 记下"这个等待者卡在哪一行"——后面唤醒要用。
3. **打破所有环**：`while (HasCycle(victim))`：把 `victim` 设 `kAborted`；从等待图里彻底删掉它（出边 + 所有人指向它的入边）；用 `waiting_on[victim]` 找到它卡的那行，`cv_.notify_all()` 唤醒——被唤醒的那个加锁线程会在 `CheckAbort` 里抛 `kDeadlock`。
4. 循环直到无环；最后 `waits_for_.clear()`。

> 这就是整条链路：**检测线程发现环 → 设牺牲品 Aborted → 通知它卡的队列 → 牺牲品线程醒来 → CheckAbort 抛 kDeadlock → 上层 Abort 释放它的锁 → 其它事务得以继续**。

---

## 四、如何体现功能 / 测试（`lock_manager_test.cpp`，10 条全过）

> 跑：`make lock_manager_test && ./test/lock_manager_test`（或在 `minisql_test` 里）。验收时可当场跑。

**基础锁与 2PL（前 6 条）：**
1. `SLockInReadUncommittedTest`：ReadUncommitted 下 `LockShared` → 抛 `kLockSharedOnReadUncommitted`，事务变 Aborted、持锁数 0。**验证隔离级别规则。**
2. `TwoPhaseLockingTest`：S(r0) 成功→GROWING；X(r1) 成功→仍 GROWING；`Unlock(r0)`→进入 SHRINKING；此后再 `LockShared` → 抛 `kLockOnShrinking`。**验证 2PL 两阶段。**
3. `UpgradeLockInShrinkingPhase`：S 后 Unlock 进入收缩，再 `LockUpgrade` → `kLockOnShrinking`。**验证升级也受 2PL 约束。**
4. `UpgradeConflictTest`：两个事务都持 r 的 S 锁、都想升级 → 其中一个抛 `kUpgradeConflict`。**验证同行只允许一个升级。**
5. `UpgradeTest`：单事务 S→X 升级成功。**验证升级正常路径。**
6. `UpgradeAfterAbortTest`：升级相关的中止场景。

**环检测（纯图，无线程）：**
7. `BasicCycleTest1`：加边 `0→1, 1→0`，`HasCycle` 返回 true 且最年轻=`1`；删 `1→0` 后无环。
8. `BasicCycleTest2`：8 条边含多个环，第一次最年轻=`5`；删 `5→1` 后变 `6`；再删 `6→0` 后无环。**验证 DFS 取最年轻、且能逐个打破。**

**真并发死锁（多线程）：**
9. `DeadlockDetectionTest1`（**重点，下面拆解**）。
10. `DeadlockDetectionTest2`：4 个事务先各持一行 S 锁，再交叉申请 X 锁形成环；检测线程把 `txn[2]`、`txn[3]` 判死锁中止（它们 catch 到 `kDeadlock`），`txn[0]`、`txn[1]` 顺利提交。**验证多事务并发死锁的检测与解除。**

### DeadlockDetectionTest1 执行过程拆解（建议背下来）

两个事务 t0、t1，两行 r0、r1：
1. t0 线程：`LockExclusive(t0, r0)` 成功（拿到 r0 的 X 锁），睡 100ms。
2. t1 线程：`LockExclusive(t1, r1)` 成功（拿到 r1 的 X 锁），睡 100ms。
3. t0 醒来：`LockExclusive(t0, r1)` —— r1 被 t1 占着，**阻塞**在 r1 的 `cv_`。
4. t1 醒来：`LockExclusive(t1, r0)` —— r0 被 t0 占着，**阻塞**在 r0 的 `cv_`。
5. 现在 **t0 等 r1（t1 持有）、t1 等 r0（t0 持有）** → 互相等待 = 死锁。
6. 后台 `RunCycleDetection` 每 500ms 跑一次：重建等待图得到 `t0→t1, t1→t0`，`HasCycle` 发现环、最年轻是 **t1**（txn_id 更大）。
7. 检测线程把 t1 设 Aborted，`notify` t1 卡着的 r0 队列。
8. t1 线程醒来，在 `LockExclusive` 里的 `CheckAbort` 抛 `kDeadlock` → 测试 catch 到、断言 `t1` 状态是 Aborted、然后 `Abort(t1)` 释放 t1 的所有锁。
9. t1 释放 r1 后，t0 在第 3 步阻塞处被唤醒、拿到 r1、继续 `Commit(t0)` 成功。
10. 结果：**t1 被牺牲、t0 提交**，死锁被解除。

---

## 五、最可能的追问 & 标准答法

- **Q：2PL 的两个阶段？** GROWING 只加不放；一旦放过任何一把锁就进入 SHRINKING，之后只放不加。这保证冲突可串行化。代码里 `Unlock` 第一次释放就把 GROWING 改成 SHRINKING。
- **Q：S 锁 / X 锁的相容性？** S-S 相容（`sharing_cnt_` 计数，多个并存）；S-X、X-X 互斥（`is_writing_` 独占）。
- **Q：升级为什么要 `is_upgrading_`？** 防止同一行上两个事务同时升级——两个都让出 S、都等对方放 S，会死等。所以同时只允许一个升级，第二个直接 `kUpgradeConflict`。
- **Q：被阻塞的线程怎么"被中止"？** 检测线程持 latch 把牺牲品状态改 Aborted 再 `notify_all`；线程的 `cv_.wait` 谓词里有"状态==Aborted"，所以会醒，醒来在 `CheckAbort` 抛 `kDeadlock`。
- **Q：`cv_.wait` 和 `latch_` 会不会互卡？** 不会。`cv_.wait` 在等待时**自动释放** latch、被唤醒时**重新获取**，所以检测线程能在牺牲品 wait 期间拿到 latch 改它状态、发通知。
- **Q：为什么牺牲最年轻？** 浪费工作最少，且固定策略避免活锁。代码里 `HasCycle` 取环中 `max(txn_id)`。
- **Q：等待图怎么来的？** `RunCycleDetection` 每个周期从锁表重建：每个等待者(granted==kNone) 向同一行的每个持有者(granted!=kNone) 连边。
- **Q：DFS 为什么结果确定？** 起点和邻居都按 txn_id 升序（`std::set` + 收集到 `set nodes`），所以"第一个找到的环"是确定的，和参考测试期望一致。
- **Q：`HasCycle` 找到环后返回的是环里的点还是路径里的点？** 是**环里**的——`path` 中从回边目标 v 到末尾那一段才是环，只在这段里取 max。

---

## 五点五、与 TxnManager / Txn 的衔接（老师很可能问，务必会答）

文档反复强调：LockManager 只管"加锁/抛异常/设状态"，**真正的回滚和最终释放由 TxnManager 做**。这条衔接关系要讲清：

- **`Txn`（给定）**：携带 `txn_id_`、隔离级别、状态机（GROWING/SHRINKING/COMMITTED/ABORTED）、以及 `shared_lock_set_ / exclusive_lock_set_`（该事务持有的锁集合）。你的 LM 在授予锁时 `txn->GetSharedLockSet().emplace(rid)`、放锁/升级时相应增删——**目的就是让 TxnManager 在 Commit/Abort 时知道要释放哪些锁**。
- **`TxnManager`（给定）**：`Begin()` 创建事务；`Commit()` / `Abort()` 结束事务。文档说本实验里它们不做额外事（不与 Recovery 耦合回滚），但职责上：**Commit/Abort 时遍历事务的锁集合统一释放**，这也是"严格 2PL（持锁到事务结束）"的落点——**注意：严格性是 TxnManager 管的，不是你 LM 管的**（你 LM 只保证 2PL：放锁即进 SHRINKING）。
- **完整的死锁解除链路**（把这条背熟）：
  1. 检测线程 `RunCycleDetection` 发现环 → `victim->SetState(kAborted)` 并 `notify` 它卡的队列；
  2. 牺牲品的加锁线程在 `cv_.wait` 醒来 → `CheckAbort` 抛 `TxnAbortException(kDeadlock)`；
  3. **异常被上层（执行器/调用方）捕获 → 调 `txn_mgr_->Abort(victim)`**；
  4. TxnManager 释放该事务锁集合里的所有锁；
  5. 其它原本等它的事务被唤醒、拿到锁、继续执行。

> 一句话：**LM 负责"发现并标记+通知"，TxnManager 负责"善后回滚+释放"**，两者通过 `Txn` 的状态和锁集合衔接。

---

## 五点六、文档"Note"硬性要求逐条对照（你代码都满足）

| 文档要求 | 你代码对应 | 满足 |
|---|---|---|
| DFS 确定性：起点选最小 txn_id、邻居从小到大 | `HasCycle` 用 `std::set nodes`（升序起点）+ 邻居是 `std::set`（升序） | ✅ |
| 发现环中止**最年轻**（txn_id 最大） | `HasCycle` 在环段里取 `max(txn_id)` | ✅ |
| 每次唤醒**即时重建**图、用完销毁 | `RunCycleDetection` 开头 `waits_for_.clear()` 重建、结尾再 `clear()` | ✅ |
| 中止时设 ABORTED **并抛异常** | `CheckAbort` 设 Aborted + 抛 `kDeadlock` | ✅ |
| 通知等待事务它被中止 | `RunCycleDetection` 里 `cv_.notify_all()` | ✅ |
| 一个事务可等待多个（共享锁多持有者） | 对每个 holder 都 `AddEdge(waiter, holder)` | ✅ |
| 通过 `lock_manager_test` 全部用例 | 10/10 | ✅ |
| **构建图时不为已中止事务加结点/连边** | 见下方说明 | ⚠️ 实质满足 |

**关于最后一条**：你的 `RunCycleDetection` 重建图时是按 `granted_` 区分 holder/waiter，**没有显式判 `GetState()==kAborted` 跳过**。但它在**杀掉牺牲品的同一轮里立刻把它从图中删除**（`waits_for_.erase(victim)` + 删所有指向它的入边），下一轮（间隔后）它的请求也已被清出锁表。所以**已中止事务不会参与后续找环，功能正确、10 条测试全过**。

被问到时这样答：*"我在发现环、中止牺牲品后，会立即把它从等待图中移除（出边和入边都删），因此它不参与本轮后续的环检测；下一轮重建时它的锁请求也已被清理。"*

**（可选）若想字面 100% 对齐文档**：在重建图、push 进 holders/waiters 前加一行守卫跳过已中止事务——**但这是改动已通过测试的代码，今晚若改务必重跑测试确认**：

```cpp
for (auto &req : q.req_list_) {
  Txn *rt = txn_mgr_->GetTransaction(req.txn_id_);
  if (rt != nullptr && rt->GetState() == TxnState::kAborted) continue; // 跳过已中止
  if (req.granted_ != LockMode::kNone) holders.push_back(req.txn_id_);
  else { waiters.push_back(req.txn_id_); waiting_on[req.txn_id_] = rid; }
}
```

> 建议：**测试已全过、且实质满足要求，明天就要验收，我倾向于不动代码**，用上面的话术回答即可。要改就一定重跑 `lock_manager_test`。

---

## 五点七、思考题（占本模块 30%，别漏！）

文档 7.6 要求"**具体到涉及哪些模块、哪些函数的改动**"。个人详细报告里的 7.6 已升级到函数级（执行引擎/各执行器/txn_manager/b_plus_tree 分别怎么改 + Latch Crabbing），口头答辩时挑重点讲：
- 事务边界 → `execute_engine` 开/提交事务、各 `executor` 的 `Next()` 里加锁、`txn_manager` 的 Commit/Abort 释放锁集合；
- 隔离级别 → 在执行器加放锁处按 `GetIsolationLevel` 分三档；
- B+ 树并发 → `b_plus_tree.cpp` 的 `FindLeafPage/Insert/Remove` 用页级 `RLatch/WLatch` 做蟹行协议，"加子→判安全→放祖先"。

---

## 六、验收前自检

- [ ] `make lock_manager_test && ./test/lock_manager_test` → 10/10 通过，能当场演示
- [ ] 能脱稿讲 `DeadlockDetectionTest1` 的 10 步过程
- [ ] 记牢三个异常名：`kLockSharedOnReadUncommitted`、`kLockOnShrinking`、`kUpgradeConflict`、`kDeadlock`
- [ ] 能解释 `granted_==kNone` = 等待中、`!=kNone` = 已持有（等待图区分 holder/waiter 全靠它）
- [ ] 能回答"为什么取最年轻""为什么用 is_upgrading_""cv 与 latch 怎么配合"
