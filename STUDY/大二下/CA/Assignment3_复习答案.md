# 计算机体系结构 Assignment 3 复习答案

> 教材基准：Hennessy & Patterson《Computer Architecture: A Quantitative Approach》
> 用法：作业只需自选 5 题计分（20×5=100），但本文档覆盖全部 32 题，可直接当期末 cram sheet。
> 文末附**推荐计分 5 题**的选择建议。

---

## 第一章 — 基础（Fundamental）

### Q01. RISC 的两个关键性能技术

RISC（Reduced Instruction Set Computer）的核心性能来源是两点：

**(1) 流水线（Pipelining）**
RISC 指令**定长、格式规整、寻址简单**，因此 fetch / decode / execute 各阶段工作量均衡，流水线易于实现、停顿少，CPI 可逼近 1。同时指令简单 → 单级逻辑短 → **时钟周期可以更短、主频更高**。

**(2) Load/Store 架构 + 大寄存器堆**
只有 load / store 访存，**所有算术逻辑运算都在寄存器之间进行**。带来两个好处：
- 减少访存次数，把慢速存储器访问从关键路径剥离；
- 寄存器多 → 编译器可把中间结果留在寄存器里，便于寄存器分配、循环展开等优化。

**如何提升性能（用 CPU 时间公式看）：** $\text{CPU time} = IC \times CPI \times T_c$。
- 流水线主要压低 **CPI**（接近 1），简单指令压低 **$T_c$**（提高主频）；
- Load/store 减少访存停顿，进一步压 CPI。
- 代价是 IC 略增（复杂操作要用多条简单指令），但总体 $IC\times CPI\times T_c$ 下降。这正是"让常见情况变快（make the common case fast）"的设计哲学。

---

### Q02. Dennard Scaling 与 Moore's Law 的关键观察，以及为何失效

**Moore's Law（摩尔定律）：** 单芯片上可集成的晶体管数量约每 ~2 年翻一番（最初 ~1 年，后修正为 ~2 年）。带来密度↑、单晶体管成本↓、功能↑。

**Dennard Scaling（登纳德缩放）：** 晶体管按比例缩小时，**功率密度保持不变**。因为特征尺寸缩小 $k$ 倍时，电压、电流同步下降，单位面积功率不变。所以可以"不增功率密度地提主频"，这是 2005 年前主频狂飙的根本原因。

**为何失效：**

| 定律 | 失效原因 |
|---|---|
| Dennard Scaling（约 2005–2007 失效）| 阈值电压 $V_{th}$ 无法继续随尺寸下降，**供电电压不能再降**；同时尺寸缩小后**亚阈值漏电（leakage）**急剧上升。结果功率密度反而上升 → 撞上 **Power Wall（功耗墙）**，无法再靠提频提性能 → 转向**多核**。|
| Moore's Law（持续放缓）| 逼近**原子尺度**与量子效应（隧穿、变异）；光刻难度与**晶圆厂建设/制造成本**指数上升；单晶体管成本不再持续下降。技术节点演进变慢、变贵。|

一句话：Moore 给你更多晶体管，Dennard 曾让这些晶体管"免费"提速；Dennard 先死，于是单核提频停滞，业界转向多核 / DLP / 专用加速。

---

### Q03. Amdahl's Law 的结论与加速比计算

**结论：** 对系统某一部分加速所获得的总加速，受限于**该部分占用的时间比例**。无论把它加速到多快，总加速被"未优化部分"卡住。

**单一可优化部分：**
$$\text{Speedup} = \frac{1}{(1-f) + \dfrac{f}{s}}, \qquad \lim_{s\to\infty}\text{Speedup} = \frac{1}{1-f}$$
其中 $f$ = 被优化部分占原执行时间比例，$s$ = 该部分的加速倍数。

**例 1（单部分）：** 某程序 60% 可并行（$f=0.6$），并行后该部分加速 2 倍（$s=2$）：
$$\text{Speedup} = \frac{1}{0.4 + 0.6/2} = \frac{1}{0.7} \approx 1.43$$
即使 $s\to\infty$，上限也只有 $1/0.4 = 2.5$ 倍。

**多个可优化部分：**
$$\text{Speedup} = \frac{1}{\Big(1-\sum_i f_i\Big) + \sum_i \dfrac{f_i}{s_i}}$$

**例 2（多部分）：** 30% 加速 2 倍，20% 加速 5 倍，其余 50% 不变：
$$\text{Speedup} = \frac{1}{0.5 + \dfrac{0.3}{2} + \dfrac{0.2}{5}} = \frac{1}{0.5+0.15+0.04} = \frac{1}{0.69} \approx 1.45$$

**启示：** 优化要瞄准**占比最大**的部分；这也是为什么"未并行的串行部分"决定了多核扩展的天花板。

---

### Q04. 四类并行架构（Flynn 分类）

按"指令流 × 数据流"划分：

| 类别 | 含义 | 例子 / 设计技术 |
|---|---|---|
| **SISD** | Single Instruction, Single Data | 经典单处理器（von Neumann）；可用 **ILP**（流水线、超标量）|
| **SIMD** | Single Instruction, Multiple Data | **向量处理器、GPU、多媒体扩展（SSE/AVX/NEON）**；一条指令操作多个数据 |
| **MISD** | Multiple Instruction, Single Data | 极少见，几乎无商用；有时举**脉动阵列（systolic array）**、容错冗余表决 |
| **MIMD** | Multiple Instruction, Multiple Data | **多核 / 多处理器 / 集群**；共享存储多处理器、消息传递集群；子类 **SPMD**（同程序多数据）|

MIMD 最灵活通用，SIMD 在数据级并行（DLP）上能效最高。

---

### Q05. 内存寻址模式（register / immediate / displacement）

以类 MIPS/RISC-V 汇编为例（字节寻址）：

| 模式 | 例子 | 含义 / 典型用途 |
|---|---|---|
| **Register（寄存器）** | `ADD R1, R2, R3` | 操作数全在寄存器；最快，用于临时变量、运算 |
| **Immediate（立即数）** | `ADDI R1, R2, #8` | 操作数是常数 8；用于常量、计数、地址偏移 |
| **Displacement（位移/基址偏移）** | `LW R1, 100(R2)` | 有效地址 = `R2 + 100`；用于数组、结构体字段、栈帧访问 |

补充常见模式：
- **Register indirect**：`LW R1, 0(R2)` —— 位移为 0 的特例，指针解引用
- **Indexed / Scaled**：`R2 + R3*size` —— 数组随机下标
- **Absolute / Immediate**：直接给地址或常数

**展示三者协作：** 取数组元素 `A[i]`（A 基址在 R2，i 在 R3）：
```
ADDI  R4, R3, #1      ; immediate：i+1
SLLI  R5, R4, #2      ; immediate 移位：(i+1)*4
ADD   R6, R2, R5      ; register：基址 + 偏移
LW    R7, 0(R6)       ; displacement：取 A[i+1]
```

---

### Q06. 即使没有技术进步，成本仍会下降的三个因素

即便制程不进步，计算机成本仍随时间下降，主要由三件事驱动：

1. **学习曲线（Learning Curve）/ 良率提升**：随着生产经验积累，制造**良率（yield）**提高，单位有效芯片成本下降。
2. **产量（Volume）**：产量翻倍约可降本一定百分比——分摊研发/固定成本、更快沿学习曲线下滑、采购议价能力更强。
3. **商品化（Commoditization）**：多供应商竞争 + 标准化，使同类产品趋同、**利润率被压薄**，价格下降。

（口诀：良率、产量、商品化。）

---

### Q07. Faults / Errors / Failures 的区别（举例）

按可靠性（dependability）定义，三者是因果链：

- **Fault（故障/缺陷）**：组件中的**潜在缺陷**（软件 bug、卡死的位、断线）。
- **Error（错误）**：fault 被激活，导致**系统内部状态出错**。
- **Failure（失效）**：error 传播到**对外可观测的服务偏离正确行为**（用户看到错误结果/崩溃）。

**软件例子：** 程序员写了一处除零判断遗漏（**fault**）。运行到某输入时执行该分支，产生错误中间值（**error**）。该错误值导致程序输出错误结果或崩溃，被用户察觉（**failure**）。

**硬件例子：** 某 DRAM 单元有制造缺陷（潜在 **fault**）→ 被访问时返回错位（**error**）→ 若 ECC 无法纠正且错误数据导致系统崩溃（**failure**）。

关键：fault 不一定变成 error（可能从不被激活），error 不一定变成 failure（可能被屏蔽/纠正，如 ECC）。

---

### Q08. 由各子系统 MTTF 计算整机 MTTF

假设各子系统**失效率恒定**（指数分布）且**相互独立**，整机为**串联系统**（任一子系统失效则整机失效）。

失效率可叠加。子系统 $i$ 的失效率 $\lambda_i = \dfrac{1}{\text{MTTF}_i}$，则
$$\lambda_{\text{sys}} = \sum_i \lambda_i = \sum_i \frac{1}{\text{MTTF}_i}, \qquad \boxed{\text{MTTF}_{\text{sys}} = \frac{1}{\sum_i \dfrac{1}{\text{MTTF}_i}}}$$

**例：** 系统含 10 个磁盘（各 MTTF = 1,000,000 h）+ 1 个电源（MTTF = 200,000 h）+ 1 个控制器（MTTF = 500,000 h）：
$$\lambda_{\text{sys}} = \frac{10}{10^6}+\frac{1}{2\times10^5}+\frac{1}{5\times10^5} = (10+5+2)\times10^{-6} = 17\times10^{-6}/h$$
$$\text{MTTF}_{\text{sys}} = \frac{1}{17\times10^{-6}} \approx 58{,}824 \text{ h}$$

**相关概念：**
- **FIT** = 每 $10^9$ 小时的失效数 = $\lambda \times 10^9$；
- **可用性 Availability** $= \dfrac{\text{MTTF}}{\text{MTTF}+\text{MTTR}}$（MTTR = 平均修复时间，MTBF = MTTF + MTTR）。

---

### Q09. RAID 0 ~ RAID 6 关键设计原则

| 级别 | 核心思想 | 冗余/容错 | 有效容量(N盘) | 备注 |
|---|---|---|---|---|
| **RAID 0** | 条带化（striping），无冗余 | 不容错 | $N$ | 性能最高，最危险 |
| **RAID 1** | 镜像（mirroring）完全复制 | 容 1 盘（每镜像组）| $N/2$ | 读快，写双写 |
| **RAID 2** | 位级条带 + Hamming ECC | 可纠错 | — | 已淘汰 |
| **RAID 3** | 字节级条带 + **专用校验盘** | 容 1 盘 | $N-1$ | 校验盘成瓶颈 |
| **RAID 4** | 块级条带 + **专用校验盘** | 容 1 盘 | $N-1$ | 小写都压校验盘（瓶颈）|
| **RAID 5** | 块级条带 + **分布式校验** | 容 1 盘 | $N-1$ | 校验分散，写性能优于 RAID 4，最常用 |
| **RAID 6** | 块级条带 + **双重分布式校验(P+Q)** | 容 **2** 盘同时失效 | $N-2$ | 大阵列重建期更安全 |

要点：从 RAID 3→4→5 是"校验粒度 / 校验位置"的演进；RAID 6 用两套独立校验换取双盘容错。

---

### Q10. 处理器性能方程

**核心方程：**
$$\text{CPU time} = IC \times CPI \times T_c = \frac{IC \times CPI}{\text{Clock Rate}}$$
$$\text{CPU time} = \text{Clock Cycles} \times T_c, \qquad \text{Clock Cycles} = IC \times CPI$$
$$CPI = \frac{\text{Clock Cycles}}{IC} = \frac{\sum_i IC_i \times CPI_i}{IC}, \qquad IPC = \frac{1}{CPI}$$

**三因子如何影响 CPU 时间：** CPU 时间是三者之积，降低任一项（其余不变）都减少 CPU 时间，但三者相互牵制：

| 因子 | 主要受谁影响 | 牵制关系 |
|---|---|---|
| **Instruction Count (IC)** | ISA、编译器 | CISC 减少 IC 但 CPI↑ |
| **CPI** | 微架构、流水线、ISA | 超标量降 CPI 但可能升 $T_c$ |
| **Clock cycle time $T_c$** | 工艺、微架构 | 深流水提主频但分支/相关使 CPI↑ |

所以不能只看主频（GHz），必须看三者乘积——这正是本书"quantitative"的核心。

---

## 附录 B & 第二章 — 存储层次（Memory Hierarchy）

### Q11. 直接映射 / 全相联 / 组相联 的设计原则

| 组织方式 | 映射规则 | 优点 | 缺点 |
|---|---|---|---|
| **Direct Mapped（直接映射）** | 每个内存块只能放进**唯一**一行：`(块地址) mod (行数)` | 硬件最简、命中快、功耗低 | **冲突 miss 高** |
| **Fully Associative（全相联）** | 块可放进**任意**一行 | 无冲突 miss，命中率最高 | 需并行比较所有 tag（CAM），**贵、慢、耗电**；只用于很小的 cache / TLB |
| **Set Associative（组相联）** | 块先定位到唯一**组**：`(块地址) mod (组数)`；组内可放任意一路 | 折中：显著降冲突 miss，成本可控 | n 路时需 n 个比较器 |

**权衡规律：** 相联度↑ → 冲突 miss↓、命中率↑，但**命中时间↑、功耗↑、成本↑**。直接映射是 1 路组相联，全相联是"组数=1"的组相联。

---

### Q12. 地址映射策略示例（字节寻址）

**地址划分：**
$$\underbrace{\text{Tag}}_{\text{标记}}\ \big|\ \underbrace{\text{Index}}_{\text{组号}}\ \big|\ \underbrace{\text{Block Offset}}_{\text{块内偏移}}$$
- Block offset 位数 $= \log_2(\text{块大小/字节})$
- Index 位数 $= \log_2(\text{组数})$，其中 **组数 $= \dfrac{\text{Cache 大小}}{\text{块大小}\times\text{相联度}}$**
- Tag 位数 $= \text{地址总位宽} - \text{Index} - \text{Offset}$

**设定：** 32 位地址，Cache = 16 KB，块大小 = 32 B。

**① 直接映射（1 路）：**
- Offset = $\log_2 32 = 5$ 位
- 行数 = 16KB/32B = 512，组数 = 512，Index = $\log_2 512 = 9$ 位
- Tag = 32 − 9 − 5 = 18 位
- 访问地址 `0x0000_1234` = `…0001 0010 0011 0100`：低 5 位 offset = `10100`(=20)；接着 9 位 index；其余 18 位 tag。用 index 选行，比 tag。

**② 4 路组相联：**
- Offset = 5 位（不变）
- 组数 = 16KB/(32B×4) = 128，Index = $\log_2 128 = 7$ 位
- Tag = 32 − 7 − 5 = 20 位
- 同一地址：用低 5 位定位块内字节，中间 7 位定位**组**，组内 4 行并行比 20 位 tag。

**③ 全相联：**
- Offset = 5 位，**Index = 0 位**（无组号）
- Tag = 32 − 5 = 27 位
- 块可放任意一行，需并行比对全部 512 行的 tag。

**对比一眼看出：** 相联度↑ → 组数↓ → Index 位↓、Tag 位↑、需要的比较器数量↑。

---

### Q13. 平均访存时间（AMAT）方程

**单级 Cache：**
$$\text{AMAT} = \text{Hit time} + \text{Miss rate} \times \text{Miss penalty}$$

**两级 Cache（嵌套展开）：**
$$\text{AMAT} = HT_{L1} + MR_{L1}\big(\underbrace{HT_{L2} + MR_{L2}^{local}\times MP_{L2}}_{\text{L1 的 miss penalty}}\big)$$

**局部 vs 全局 miss rate：**
- 局部：$MR_{L2}^{local} = \dfrac{\text{misses}_{L2}}{\text{accesses}_{L2}}$
- 全局：$MR_{L2}^{global} = \dfrac{\text{misses}_{L2}}{\text{total CPU refs}} = MR_{L1}\times MR_{L2}^{local}$

**计入 CPU 时间（访存停顿）：**
$$\text{CPU time} = IC \times \Big(CPI_{base} + \frac{\text{mem accesses}}{\text{instr}}\times MR \times MP\Big)\times T_c$$

**例：** $HT_{L1}=1$cyc，$MR_{L1}=5\%$，$HT_{L2}=10$cyc，$MR_{L2}^{local}=40\%$，$MP_{L2}=100$cyc：
$$\text{AMAT} = 1 + 0.05\times(10 + 0.4\times100) = 1 + 0.05\times50 = 3.5\text{ cyc}$$

---

### Q14. Cache 优化技术的关键思想与影响

按"降 miss rate / 降 miss penalty / 降 hit time / 增带宽"四类组织。先记 **3C 模型**：Compulsory（强制）、Capacity（容量）、Conflict（冲突）miss。

| 技术 | 关键思想 | 主要影响 |
|---|---|---|
| 增大**块大小** | 利用空间局部性 | 降 compulsory miss；但 ↑miss penalty，过大反而 ↑conflict/capacity miss |
| 增大 **Cache** | 装更多数据 | 降 capacity miss；但 ↑hit time、↑功耗成本 |
| 提高**相联度** | 减少映射冲突 | 降 conflict miss；但 ↑hit time、↑功耗 |
| **多级 Cache** | L2/L3 缓冲 | 降有效 miss penalty |
| **读优先于写**（写缓冲）| 读 miss 不等写回 | 降 miss penalty |
| **Critical word first / early restart** | 先取所需字 | 降 miss penalty |
| **合并写缓冲（merging write buffer）** | 合并相邻写 | 降 miss penalty、提带宽 |
| **非阻塞 Cache（hit under miss）** | miss 时继续服务后续命中 | 降有效 miss penalty、↑带宽 |
| **多 bank Cache** | 并行访问多体 | ↑带宽 |
| **流水化 Cache 访问** | 拆分命中路径 | ↑主频/带宽（hit time 名义↑但吞吐↑）|
| **Way prediction（路预测）** | 预测命中哪一路 | 降 hit time（接近直接映射速度）|
| **虚拟索引（避免索引前翻译）** | TLB 与索引并行 | 降 hit time |
| **编译优化**（loop interchange、blocking/分块）| 改善访问局部性 | 降 miss rate（软件层）|
| **硬件预取 / 编译器预取** | 提前取数 | 降 miss rate/penalty；但 ↑带宽/功耗，预取错则浪费 |

记忆主线：**miss rate** 看 3C，**miss penalty** 看多级/写优先/关键字优先，**hit time** 看小而简单/路预测/虚索引，**带宽** 看流水/多 bank/非阻塞。

---

### Q15. 借助 TLB 的两级 Cache 地址翻译过程

CPU 发出**虚拟地址 VA = (VPN | page offset)**：

1. **TLB 查找**：用 VPN 查 TLB。
   - **TLB 命中** → 得到 PPN（物理页号）。
   - **TLB miss** → 查页表（硬件/软件 page table walk）填回 TLB；若页不在内存 → **page fault**，从磁盘调页。
2. **形成物理地址 PA = (PPN | page offset)**（page offset 在翻译中不变）。
3. **访问 L1 Cache**：
   - 常用优化 **VIPT（virtually-indexed, physically-tagged）**：用 VA 中的 page offset 位**与 TLB 翻译并行**地索引 L1，待 TLB 给出 PPN 后再比物理 tag —— 把翻译从命中关键路径上移走，降 hit time。
   - L1 命中 → 返回数据。
4. **L1 miss → 访问 L2 Cache（通常 PIPT，物理索引物理 tag）**：用 PA 索引 L2，比 tag。
5. **L2 miss → 访问主存**，取块逐级回填 L1/L2。

流程链：**VA → TLB（→ 页表/缺页）→ PA → L1（VIPT，与 TLB 并行）→ L2（PIPT）→ 主存**。

---

## 附录 A & 附录 C & 第三章 — 指令级并行（ILP）

### Q16. 三类相关（Dependences）

1. **数据相关（Data dependence，真相关 / RAW）**：指令 j 用到指令 i 产生的结果。是程序固有属性，**必须遵守**。
2. **名字相关（Name dependence）**：两条指令用同一寄存器/内存名，但**无数据流**。两种：
   - **反相关（Antidependence，WAR）**：j 要写的位置正是 i 要读的；
   - **输出相关（Output dependence，WAW）**：i、j 都写同一位置。
   - 可通过**寄存器重命名（register renaming）**消除。
3. **控制相关（Control dependence）**：指令是否执行取决于某分支的结果（相对分支的执行次序约束）。

---

### Q17. 常见冒险类型，及数据相关与数据冒险的关系

**三类冒险（hazard）：**
- **结构冒险（Structural）**：资源冲突（同一周期争用同一硬件单元）。
- **数据冒险（Data）**：RAW、WAR、WAW。
- **控制冒险（Control）**：分支导致取指方向未定。

**数据相关 vs 数据冒险：**
- **相关（dependence）是程序本身的属性**；**冒险（hazard）是相关在具体流水线上被"暴露"出来、可能改变访问顺序**的事件。
- 相关是冒险的**必要非充分**条件：是否真的发生冒险，取决于流水线结构（重叠程度、级数）。
- 在**简单顺序流水线**中只会出现 **RAW**；**WAR、WAW** 只有在**乱序执行**或某些写多端口/不同步流水结构下才出现。

| 数据相关 | 对应数据冒险 |
|---|---|
| 真相关 | RAW（Read After Write）|
| 反相关 | WAR（Write After Read）|
| 输出相关 | WAW（Write After Write）|

---

### Q18. 处理数据冒险的常用前递（Forwarding）方案

**前递 / 旁路（Forwarding / Bypassing）：** 不等结果写回寄存器堆，直接把上一条指令产生的结果从流水线寄存器**直接送回**后续指令的输入端。

经典 5 级流水线常见前递路径：
- `EX/MEM → EX`（ALU 结果直接送下一条 ALU 输入）
- `MEM/WB → EX`（隔一条的结果）
- **寄存器堆同周期"先写后读"**（前半周期写、后半周期读）解决相隔较远的 RAW。

**Load-use 冒险：** load 结果要到 **MEM** 阶段末才有，前递无法完全消除 → 需**插入 1 个 stall（气泡）**，再从 `MEM/WB → EX` 前递。编译器可通过指令调度把无关指令填进这个空。

实现上由**冒险检测单元（hazard detection unit）**比较各级流水线寄存器中的源/目的寄存器号来选择前递来源。

---

### Q19. 常见分支预测器及其属性（考虑分支数 / 存储成本）

| 预测器 | 思想 | 考虑几条分支 | 存储成本 |
|---|---|---|---|
| **静态预测** | 总是 taken / not-taken / BTFN（后向 taken、前向 not-taken）| 单条 | ~0 |
| **1-bit 预测** | 记上次结果 | 单条 | 1 bit × 表项；循环每轮**误判 2 次**（首尾）|
| **2-bit 饱和计数** | 需连续两次错才翻转 | 单条 | 2 bit × 表项；循环只**误判 1 次**，常用 |
| **相关/两级预测 (m,n)** | 用前 **m** 条分支历史，从 $2^m$ 套 n-bit 预测器中选 | **多条**（全局相关）| $2^m \times n \times$ 表项 |
| **gshare** | 全局历史寄存器 XOR PC 索引 | 多条 | 历史寄存器 + PHT |
| **局部预测（local）** | 每条分支自己的历史 → 模式表 | 单条（自身历史模式）| 局部历史表 + PHT |
| **锦标赛预测（tournament）** | 用一个选择器在"局部"和"全局"预测器间动态择优 | 多条 | 两套预测器 + 选择器，成本最高、精度最高 |

另配 **BTB（Branch Target Buffer）** 缓存分支目标地址，便于取指阶段就知道跳哪。
属性总结：历史位越多、考虑分支越多 → 精度越高，但**存储随历史长度指数增长（$2^{\text{history}}$）**。

---

### Q20. 超标量（Superscalar）的思想及对 IPC/CPI 的作用

**思想：** 每周期**取/译/发射多条指令**（发射宽度 > 1），配多套功能单元/多条流水线，让多条**相互独立**的指令并行执行。

**对性能的作用：**
- 单发射流水线理想 CPI = 1（IPC = 1）；超标量把理想 **IPC 提到发射宽度**（如 4 发射 IPC 上限 = 4），即 **CPI 降到 1 以下**。
- 本质是挖掘程序中的 **ILP**。

**两类：**
- **静态超标量 / VLIW**：编译器静态打包并行指令；
- **动态超标量**：硬件动态调度（Tomasulo 等）。

**限制：** 实际 IPC 受数据/控制相关、结构冒险、可挖掘的 ILP 数量、取指带宽限制，远低于理想发射宽度。

---

### Q21. Scoreboarding / Tomasulo / Tomasulo+推测 各步骤的具体操作（重点：何时如何处理各类冒险）

#### A) Scoreboarding（CDC 6600）—— 无重命名、无前递
集中式记分牌跟踪：指令状态、功能单元状态、寄存器结果状态。四步：

1. **Issue（发射）**：若所需 **FU 空闲** 且 **没有其他在飞指令写同一目的寄存器**（避免 **WAW**）→ 发射。否则停顿（**结构冒险**或 **WAW** 在此处挡）。
2. **Read Operands（读操作数）**：等到**没有更早指令将要写该源寄存器**时（解决 **RAW**），从寄存器堆读出两个操作数。
3. **Execution（执行）**：FU 执行，完成后通知记分牌。
4. **Write Result（写结果）**：写之前检查 **WAR** —— 等到**所有更早、要读该目的寄存器的指令都已读完**，才写回。

> 冒险处理：结构 & WAW 在 **Issue**；RAW 在 **Read Operands**；WAR 在 **Write Result**。无重命名 → WAR/WAW 会造成停顿；无前递 → 必须写回后才能读。

#### B) Tomasulo's Algorithm（IBM 360/91）—— 保留站 + 重命名 + CDB
关键部件：**保留站（Reservation Station, RS）**、通过 RS 标签实现**寄存器重命名**、**公共数据总线（Common Data Bus, CDB）**。三步：

1. **Issue（发射/派遣）**：从指令队列取指；若对应类型 **RS 有空位** → 发射。**操作数已就绪则读值，否则记录将产生它的 RS 标签（Qj/Qk）** —— 这一步用标签做**重命名，消除 WAR 与 WAW**。仅当 **RS 满**（结构冒险）才停顿。
2. **Execute（执行）**：监听 CDB，**两个操作数都就绪后开始执行**（解决 **RAW**）。load/store 先算有效地址，做内存相关消解。
3. **Write Result（写结果）**：结果就绪 → **带标签广播到 CDB**；所有等待该标签的 RS 和寄存器同时抓取；并更新寄存器堆（仅当它是该寄存器的最新写者）。

> 冒险处理：结构在 **Issue**；RAW 靠 **CDB 标签匹配**；**WAR/WAW 已被重命名消除**，无停顿。

#### C) Tomasulo + 推测（加 ROB，支持分支推测 + 顺序提交）
在 B 基础上加 **重排序缓冲 ReOrder Buffer（ROB）**，重命名改由 ROB 项承担，结果先进 ROB、**顺序提交**。四步：

1. **Issue**：需 **RS 有空位且 ROB 有空项** 才发射；操作数取值或记 ROB 标签。
2. **Execute**：操作数就绪即执行，**可越过未决分支推测执行**。
3. **Write Result**：广播 CDB，写到等待的 RS 和 **ROB（不直接写寄存器堆）**。
4. **Commit（提交/退休）**：**按程序顺序**从 ROB 头部提交：
   - 正常指令 → 写回寄存器堆（store 写内存），释放 ROB 项；
   - **分支预测错** → **清空 ROB**、从正确目标重启。

> 推测执行 + 顺序提交 → 既能越分支挖 ILP，又能保证**精确异常**和错误恢复。寄存器只在 **commit** 时才被真正更新。

---

### Q22. 给定指令序列，确定不同动态调度算法的时间线

**假设（务必写明）：**
- 每周期最多发射 1 条；CDB 每周期最多广播 1 个结果。
- 执行周期数：**Load = 2，FP MUL = 4，FP ADD/SUB = 2**。
- 结果在第 $C$ 周期写出后，依赖它的指令**最早第 $C{+}1$ 周期**开始执行（即写与被捕获不在同一周期）。
- 功能单元足够，不考虑结构冒险。
- Tomasulo 有 CDB 前递；Scoreboarding 无前递（必须写回寄存器后才能 Read Operands）。

**指令序列：**
```
I1:  L.D    F2, 0(R1)
I2:  MUL.D  F4, F2, F2      ; RAW on F2 (依赖 I1)
I3:  ADD.D  F6, F4, F2      ; RAW on F4 (依赖 I2)
```

**Tomasulo 时间线：**

| 指令 | Issue | Execute | Write Result |
|---|---|---|---|
| I1 L.D | c1 | c2–c3 | c4 |
| I2 MUL.D | c2 | c5–c8（等 F2，c4 广播后 c5 起）| c9 |
| I3 ADD.D | c3 | c10–c11（等 F4，c9 广播后 c10 起）| c12 |

**Scoreboarding 时间线（多一个 Read Operands 阶段，且无前递）：**

| 指令 | Issue | Read Op | Execute | Write Result |
|---|---|---|---|---|
| I1 L.D | c1 | c2 | c3–c4 | c5 |
| I2 MUL.D | c2 | c6（等 I1 写回 c5）| c7–c10 | c11 |
| I3 ADD.D | c3 | c12（等 I2 写回 c11）| c13–c14 | c15 |

**结论：** Tomasulo 在 c12 完成，Scoreboarding 在 c15 完成。差距来自：Tomasulo 通过 CDB 把结果**前递**给等待的指令，结果写出后**下一周期**即可执行；Scoreboarding 需先写回寄存器堆、再用独立的 Read Operands 阶段读取。

**重命名优势补充（WAR/WAW 对比）：** 若序列含
```
I1: DIV.D F0,F2,F4   (长延迟)
I2: ADD.D F6,F0,F8   (RAW on F0)
I3: SUB.D F8,F10,F12 (WAR on F8：I2 要读 F8)
I4: MUL.D F6,F10,F8  (WAW on F6 与 I2；RAW on F8 与 I3)
```
- **Scoreboarding**：I3 的 **Write Result 必须等 I2 读完 F8**（WAR），而 I2 又被长延迟 DIV 卡住 → I3 写回被拖，进而 I4 停顿。
- **Tomasulo**：I2 在 Issue 时已把 F8 的值/标签记入 RS（重命名），I3 写自己的结果**无需等待**（WAR 消除）；I4 通过标签获取 I3 的 F8，并通过 ROB/最新写者机制解决与 I2 的 WAW。→ 明显更快。

---

## 第四章 — 数据级并行（DLP）

### Q23. 三类常用 DLP 架构

1. **向量架构（Vector architectures）** —— 经典 Cray 式，向量寄存器 + 深流水 FU。
2. **SIMD 多媒体扩展（SIMD multimedia extensions）** —— MMX/SSE/AVX、ARM NEON 等，短向量指令。
3. **GPU（图形处理器）** —— 大量轻量线程（SIMT），高吞吐。

---

### Q24. 向量架构的工作原理

**核心：一条向量指令处理整个一维数组（向量）。**

**关键部件：**
- **向量寄存器（vector registers）**：每个可存 N 个元素（如 64）；
- **深度流水的向量功能单元（vector FUs）**；
- **向量 load/store 单元**（规则访存，利于多 bank）；
- 标量寄存器；多 **lane（车道）** 并行。

**为什么快：**
- 一条指令 = 多次运算 → **摊薄取指/译码开销，消除循环开销**；
- 向量元素**相互独立** → 流水内无数据冒险，可全速流水；
- 访存模式**规则**（unit-stride）→ 适合多 bank 交叉访存，隐藏访存延迟。

**关键机制：**
- **Strip mining（条带挖掘）**：向量长度 > 寄存器长度时分段处理；
- **VLR（Vector Length Register）**：处理非满长向量；
- **Vector Mask Register（向量掩码）**：实现条件执行（IF 转换）；
- **Stride（步长）/ Gather-Scatter（聚散）**：非连续/索引访存；
- **Chaining（链接）**：见 Q26；
- **Multiple lanes（多车道）**：并行处理多个元素。

---

### Q25. 动态寄存器分型（Dynamic register typing）的核心思想

> 注：此为课程/讲义术语，下述为最常见解释，请对照你的课件确认。

**核心思想：向量寄存器堆的总存储固定，但其"如何切分为多少个寄存器、每个多长"可以动态配置。**

也就是说，ISA 不把"向量寄存器个数 × 每个长度"写死。给定一块固定容量的向量寄存器存储，可以**动态地**在两种用法间权衡：
- 需要**更长向量**时 → 配成**少而长**的寄存器（提高单条指令的并行度、降低开销）；
- 需要**更多寄存器**时（如复杂循环、多个活跃向量）→ 配成**多而短**的寄存器。

好处：提高寄存器堆利用率，并使代码具有**向量长度无关（vector-length-agnostic）**的可移植性——同一份程序可在不同向量长度的实现上运行（思想上类似 RISC-V V 扩展的可配置向量长度）。

（另一种语境下指 **GPU 的动态寄存器分配**：寄存器堆按每线程实际用量动态分配，寄存器用得越少、可并发线程越多、occupancy 越高。若你的课件偏 GPU，请采用此解释。）

---

### Q26. 向量执行时间如何计算？取决于什么？为何链接(chaining)能优化？
How to compute vector execution time? What factors does it depend on? Why does chaining helps to optimize vector execution time?
**关键概念：**
- **Convoy（车队）**：一组**可一起开始执行、彼此无结构/数据冒险**的向量指令。
- **Chime（钟鸣）**：执行一个 convoy 所需的时间单位 ≈ **向量长度 n 个周期**（忽略启动开销）。

**执行时间（近似）：** 若有 $m$ 个 convoy、向量长 $n$、各 convoy 启动开销 $T_{start}$：
$$T_{total} \approx m \times n + \sum T_{start} \quad\Big(\text{粗略：}\ T \approx m\cdot n\ \text{chimes}\Big)$$
更精确地按 convoy 累加：$T = \sum_{i=1}^{m}\big(T_{start,i} + n\big)$。

**取决于：**
1. **向量长度 n**；
2. **convoy 数 m**（由结构冒险和数据相关决定）；
3. **启动开销 / 流水线深度**（FU 延迟、访存延迟）；
4. **车道数（lanes）**：n/lanes，可缩短每 convoy 时间。

**为何 chaining 能优化：**
没有链接时，存在 RAW 的两条向量指令必须分到**不同 convoy**（前者全部写完才能启动后者）→ convoy 数↑。
**链接**允许后续指令在**前一向量结果的第一个元素一产生**就开始消费（逐元素前递），从而让有相关的指令**进入同一 convoy 并行流动** → **减少 convoy 数 / chime 数** → 缩短总执行时间。直观上像把多个向量 FU"首尾接成一条更长的流水线"。

**例：** `MULV V1,V2,V3` 后接 `ADDV V4,V1,V5`（RAW on V1）。无链接：2 个 convoy ≈ 2 chime。有链接：合成 1 个长流水 convoy，仅多付一次额外启动延迟，≈ 1 chime + 开销。

---

### Q27. Stride（步长）如何影响多 bank 内存的访问延迟（举例）

**机制：** 多 bank（交叉编址）内存把连续地址分散到不同 bank，可并行/流水访问。每个 bank 有 **bank busy time**（访问后需忙若干周期才能再次服务）。若**步长使连续访问落到尚在忙的同一 bank** → **bank 冲突**，必须等待。

**例：** 设 **16 个 bank**，**bank busy time = 8 周期**，bank 编号 = `地址 mod 16`，连续取一个向量元素。

- **Stride = 1：** 依次访问 bank 0,1,2,…,15,0,…。同一 bank 每隔 **16** 次访问才重用，而 busy 只有 8 周期 < 16 → **无冲突**，可全速流水，平均每周期 1 个元素。
- **Stride = 8：** 访问 bank 0,8,0,8,…，同一 bank 每隔 **2** 次就重用，但 busy = 8 周期 > 2 → **严重冲突**，每次都要等 bank 空闲，吞吐骤降（有效 bank 数被压到 16/gcd(8,16)=2）。
- **一般规律：** 有效并行 bank 数 $= \dfrac{\#banks}{\gcd(\text{stride},\ \#banks)}$。当 **stride 与 bank 数互质**时最优；当 **stride 是 bank 数的倍数**（如 stride=16）时，全部落同一 bank，退化为串行。

**启示：** 这就是为什么 unit-stride 访问最理想，以及为什么 bank 数常取**质数**或要求 stride 与 bank 数互质。

---

### Q28. 循环携带相关（Loop-carried dependence）及消除改写（举例）

**定义：** 循环携带相关 = **不同迭代之间**的相关（第 i 次迭代依赖第 i−1 次的结果），它阻碍并行化/向量化。

**含循环携带相关的例子（H&P 经典）：**
```c
for (i = 0; i < 100; i++) {
    A[i]   = A[i] + B[i];     /* S1：读 B[i] */
    B[i+1] = C[i] + D[i];     /* S2：写 B[i+1] */
}
```
分析：S1 在第 i+1 次迭代读 `B[i+1]`，而它由 S2 在第 i 次迭代产生 → **S2 → S1 的循环携带相关**（跨迭代）。注意这条相关**不是循环的（非环）**，因此可以改写消除。

**改写以消除（把首尾"剥离"，使循环体内只剩同迭代相关）：**
```c
A[0] = A[0] + B[0];                 /* 处理首项 */
for (i = 0; i < 99; i++) {
    B[i+1] = C[i] + D[i];           /* 先算本迭代的 B[i+1] */
    A[i+1] = A[i+1] + B[i+1];       /* 同迭代内使用，无跨迭代相关 */
}
B[100] = C[99] + D[99];             /* 处理尾项 */
```
改写后循环体内 S2、S1 都只用**本次迭代**的数据，**无循环携带相关** → 各迭代相互独立，可向量化/并行化。

（对比真正不可消除的环型相关，如 `A[i] = A[i-1] + B[i]`：S1 自身跨迭代成环，无法简单向量化，需扫描/前缀和等专门算法。）

---

## 第五章 — 线程级并行（TLP）

### Q29. 多核系统中通信开销如何影响性能

多核性能不仅看计算，还要看**核间通信开销**。影响途径：

1. **远程访问延迟（latency）**：访问其他核/远端内存比本地慢得多，核因此**停顿**。
2. **带宽限制（bandwidth）**：互连网络/内存带宽有限，通信量大时排队。
3. **竞争与拥塞（contention）**：多核争用总线/网络/目录，延迟进一步上升。
4. **一致性流量（coherence traffic）**：失效/更新广播、目录消息本身消耗带宽。
5. **占用（occupancy）**：发送/接收消息占用处理器周期。

**量化思路：**
$$CPI_{total} = CPI_{base} + (\text{远程访问频率}) \times (\text{每次远程访问的开销周期})$$
通信越多、单次越贵 → 等效 CPI↑ → 性能↓。

**对扩展性的影响：** 若通信量随核数增长（通信/计算比上升），加速比会**饱和**（呼应 Amdahl）。优化方向：**提高数据局部性、减少共享与通信、用更好的互连**。

---

### Q30. 内存系统"一致（coherent）"的条件

一个内存系统是**一致的**，当且仅当满足三条性质：

1. **保持单处理器程序序**：处理器 P 写 X 后再读 X，**其间无其他处理器写 X**，则读到 P 自己写的值（处理器看得见自己的写）。
2. **写传播（write propagation / coherent view）**：处理器 P1 写 X 之后，**经过足够时间间隔**，另一处理器 P2 读 X（其间无别的写）应读到 P1 写的值（写最终对其他核可见）。
3. **写串行化（write serialization）**：对**同一地址**的多个写被**串行化**——所有处理器看到对该地址的写**顺序一致**（不会有两个处理器对同一地址观察到相反的写序）。

> 一致性（coherence）管"对单个地址的值是否最新、写序是否一致"；一致性模型（consistency，Q32）管"对不同地址的访问之间的可见次序"。两者不同。

---

### Q31. 监听 vs 目录 一致性协议处理访存请求示例

**系统设定：** 3 个核 P0/P1/P2，各有一个私有写回(write-back) cache，**MSI 协议**（Modified / Shared / Invalid），单块 A 初始只在内存，三核均为 **I**。

**访存序列：**
1. P0 读 A
2. P1 读 A
3. P0 写 A
4. P2 读 A

#### (a) 监听式（Snooping，invalidate）——各 cache 监听总线

| 步 | 操作 | 总线事务 | P0 | P1 | P2 |
|---|---|---|---|---|---|
| 0 | 初始 | — | I | I | I |
| 1 | P0 读 A（miss）| BusRd，内存供数 | **S** | I | I |
| 2 | P1 读 A（miss）| BusRd，内存/P0 供数 | S | **S** | I |
| 3 | P0 写 A | **BusRdX/BusUpgr**，使其他失效 | **M** | **I** | I |
| 4 | P2 读 A（miss）| BusRd，**P0(M) 供数并写回**，降为 S | **S** | I | **S** |

要点：写时**广播失效**别人；别人持 M 时由**该核供数**（cache-to-cache）并写回。

#### (b) 目录式（Directory）——主目录记录 {状态, 共享者位向量}

| 步 | 操作 | 发给主目录的消息 | 目录状态 | 共享者 | P0 | P1 | P2 |
|---|---|---|---|---|---|---|---|
| 0 | 初始 | — | Uncached | {} | I | I | I |
| 1 | P0 读 | Read miss | Shared | {P0} | **S** | I | I |
| 2 | P1 读 | Read miss | Shared | {P0,P1} | S | **S** | I |
| 3 | P0 写 | Write/Upgrade | Modified（owner=P0）| {P0} | **M** | **I**（目录发失效给 P1）| I |
| 4 | P2 读 | Read miss | Shared | {P0,P2} | **S**（目录令 P0 写回）| I | **S** |

要点：目录用**点对点消息**而非广播 → **可扩展**；写时目录**只向当前共享者发失效**；读到被 M 持有的块时，目录**指挥拥有者写回/转发**。

**两协议对比：** 监听靠**广播 + 总线串行化**，简单但**不易扩展**；目录靠**集中/分布式目录 + 定向消息**，扩展性好但目录存储与协议更复杂。

---

### Q32. 不同一致性模型(consistency)所要求的次序

访问次序有四种程序序约束：**R→R、R→W、W→R、W→W**（前者读/写 必须在 后者读/写 之前完成）。各模型要求/放松情况：

| 一致性模型 | R→R | R→W | W→R | W→W | 说明 |
|---|---|---|---|---|---|
| **顺序一致性 SC** | ✓ | ✓ | ✓ | ✓ | 最严格：存在单一全局序，所有处理器看到一致；性能差 |
| **TSO / 处理器一致性** | ✓ | ✓ | ✗（放松）| ✓ | 允许**写缓冲**：后续读可越过对**不同地址**的更早写（W→R 放松）|
| **PSO（部分存储序）** | ✓ | ✓ | ✗ | ✗（放松）| 进一步允许写与写重排（W→W 放松）|
| **弱序 / 释放一致性 (Weak / Release)** | ✗ | ✗ | ✗ | ✗ | 普通访存**全部可重排**，仅在**同步点（fence / acquire-release）**强制次序 |

记忆主线：从 **SC → TSO → PSO → Weak/Release**，逐步放松四种次序以换取性能，代价是程序员必须**显式插入内存屏障（memory fence）**来保证正确性。

---

## 📌 推荐用于计分的 5 题

作业要你"指定 5 题计分"。建议挑**带具体计算/例子、易于展示掌握程度**且**覆盖五章**的题（这类题最好拿分）：

| 推荐题 | 章节 | 为什么适合计分 |
|---|---|---|
| **Q03** Amdahl's Law | Ch1 | 有公式 + 多部分计算，步骤清晰 |
| **Q12** Cache 地址映射 | Ch2 | 三种组织的完整数值推导，最能体现理解 |
| **Q22** 动态调度时间线 | Ch3 | Tomasulo vs Scoreboarding 时间线 + WAR/WAW 对比，分量足 |
| **Q26** 向量执行时间 / chaining | Ch4 | convoy/chime 公式 + 链接优化解释 |
| **Q31** 一致性协议示例 | Ch5 | 监听 vs 目录 两张状态转移表，直观 |

这套覆盖 Ch1–Ch5 全部章节，且都是"能写出过程"的题，给分友好。当然你也可以按自己最有把握的来挑。

---

*以上 32 题均按 H&P 教材标准答案组织。Q25（动态寄存器分型）属课程术语，已给最可能的两种解释，建议对照你的课件确认；Q22 的时间线依所列假设而定，若你们课上对"发射/广播能否同周期""前递规则"有不同约定，数字需相应平移。*
