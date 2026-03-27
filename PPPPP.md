- 1.信息处理

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

2.对比

| 维度    | ANN          | INT4/INT8          | SNN            | NSLLM         |
| ----- | ------------ | ------------------ | -------------- | ------------- |
| 信息表示  | 连续值（float）   | 低比特整数              | spike（0/1）     | 整数 + spike    |
| 精度来源  | 数值幅度         | 数值幅度（低bit）         | 时间/频率          | 时间展开          |
| 计算方式  | Dense MatMul | Dense MatMul（低bit） | 事件驱动           | Add/Sub + 时间步 |
| 是否有乘法 | ✅            | ✅（低bit）            | ❌              | ❌             |
| 稀疏性   | ❌            | ❌（弱）               | ✅              | ✅             |
| 时间维度  | ❌            | ❌                  | ✅（真实时间）        | ✅（计算时间）       |
| 执行方式  | 同步           | 同步                 | 异步             | 同步            |
| 硬件友好性 | GPU友好        | GPU友好              | Neuromorphic芯片 | FPGA/ASIC友好   |
| 能效    | ❌            | ⬆️                 | ⬆️⬆️           | ⬆️⬆️          |
| 延迟    | 低            | 低                  | 不稳定            | 较高            |
| 训练难度  | 中            | 低                  | 高              | 中             |

- 3.硬件执行：

| 方法    | 硬件执行逻辑          |
| ----- | --------------- |
| ANN   | 矩阵乘法阵列          |
| INT4  | 低bit矩阵乘         |
| SNN   | 事件路由 + neuron更新 |
| NSLLM | 时间步循环 + 加减运算    |
- 4.综合性能
Compute Cost ≈ bit-width × active operations × time-steps

*“demonstrates a 19.8 improvement in power efficiency, a 21.3 reduction in memory usage and a 2.2 increase in inference throughput.”*

> 我认为 NSLLM 是在 ANN 和 SNN 之间寻找一种折中方案来降低大模型的计算和内存成本。
> 
> 首先在信息表示上，它没有直接使用传统 SNN 的二值脉冲，而是先将连续激活量化为整数，再通过时间展开转化为二值脉冲序列，让信息通过时间维度进行编码。
> 
> 其次在计算上，NSLLM利用脉冲信号的 0/1 特性以及低比特权重（例如{-1，0，1}），将乘法操作转化为加、减或跳过，从而降低计算复杂度。
> 
> 在硬件部署层面，NSLLM没有采用传统神经拟态的事件驱动执行方式，而是仍然采用同步的数据流计算模式，通过时间步展开来处理脉冲序列，并利用稀疏性减少计算。
> 
> 所以NSLLM 可以看作是在保持 ANN 可扩展性的同时，引入 SNN 的稀疏和硬件友好特性的一种折中设计。

**NSLLM = bit-serial computing + sparse computing**

>而且NSLLM实际上是融合两种现有技术bit-serial computing和sparse computing的组合，把脉冲神经（通过数字逻辑）、稀疏计算以及按位计算运用到LLM上。



//
*“As depicted in Fig. 1a, we meticulously monitored the quantization errors of spiking neurons and quantized the full-precision activation values and weights within predefined integer intervals .”*
*“Based on this, we proposed a layer-wise sensitivity-based algorithm to determine the optimal quantization mode for each layer in the neural network.”*

![[Pasted image 20260327215110.png]]
