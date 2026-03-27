| 维度    | ANN             | INT4/INT8          | SNN            | NSLLM         |
| ----- | --------------- | ------------------ | -------------- | ------------- |
| 信息表示  | 连续值（float）      | 低比特整数              | spike（0/1）     | 整数 + spike    |
| 精度来源  | 数值幅度            | 数值幅度（低bit）         | 时间/频率          | 时间展开          |
| 计算方式  | Dense MatMul    | Dense MatMul（低bit） | 事件驱动           | Add/Sub + 时间步 |
| 是否有乘法 | ✅               | ✅（低bit）            | ❌              | ❌             |
| 稀疏性   | ❌               | ❌（弱）               | ✅              | ✅             |
| 时间维度  | ❌               | ❌                  | ✅（真实时间）        | ✅（计算时间）       |
| 执行方式  | 同步              | 同步                 | 异步             | 同步            |
| 硬件友好性 | GPU友好           | GPU友好              | Neuromorphic芯片 | FPGA/ASIC友好   |
| 能效    | ❌               | ⬆️                 | ⬆️⬆️           | ⬆️⬆️          |
| 延迟    | 低               | 低                  | 不稳定            | 较高            |
| 训练难度  | 中               | 低                  | 高              | 中             |


```
      ANN（传统）             NSLLM（本文）                SNN（传统神经拟态）

输入 x (float)            输入 x (float)              输入 spike (0/1)
      │                         │                           │
      ▼                         ▼                           ▼
[连续激活]                 [连续激活]                [膜电位 V]
      │                         │                           │
      ▼                         ▼                           ▼
（直接计算）            STEP1：量化                spike 到来
 y = W × x                ↓                        │
      │              [整数 spike]                 ▼
      ▼                         │            V ← V + w
（矩阵乘法）                 ▼                    │
      │              STEP2：时间展开             ▼
      ▼                ↓                  V ≥ threshold ?
输出 y (float)     [二值 spike 序列]          │
                          │                 是 → 发 spike
                          ▼                 │
                  加/减/跳过计算              ▼
                          │             事件传播
                          ▼
                    输出（累加）

```


- 硬件执行：

| 方法    | 硬件执行逻辑          |
| ----- | --------------- |
| ANN   | 矩阵乘法阵列          |
| INT4  | 低bit矩阵乘         |
| SNN   | 事件路由 + neuron更新 |
| NSLLM | 时间步循环 + 加减运算    |
Compute Cost ≈ bit-width × active operations × time-steps

**NSLLM = bit-serial computing + sparse computing**
