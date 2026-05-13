# Progress: SNN文献阅读进度日志

---

## 2025年5月上旬 - 第一篇论文：E-SpikeFormer精读

### 完成内容
- 精读论文"Scaling Spike-driven Transformer with Efficient Spike Firing Approximation Training"（TPAMI 2025）
- 深入理解了SNN vs ANN的核心矛盾（省电但不准 vs 准但耗电）
- 掌握了二值脉冲的两个先天缺陷：空间表示差（Eq.2 Heaviside函数丢失信号强度）、时间动态差（Eq.3软重置只能遗忘固定值Vth）
- 掌握了SFA三步法（Eq.4-7）及其数学推导
- 理解了Proposition 1的证明：IF-SR神经元每步减1，整数3→连续发射3个脉冲后停止
- 理解了近似误差分析（Eq.25/28）：三段讨论，D越大误差越小
- 理解了E-SpikeFormer架构改进：SpikeSepConv替换RepConv消除60%能耗
- 理解了SSC + MIM预训练解决SNN扩展退化（论文唯一完全首次的创新点）
- 制作了18页技术详细版PPT（含公式推导、Proposition 1证明、误差分析、架构图）

### 关键认知
- SFA的核心想法（整数训练）来自Luo et al.的I-LIF（SpikeYOLO），E-SpikeFormer的贡献是理论化和系统化
- "完全首次"的创新点只有SSC+MIM预训练，其余是系统化贡献
- 静态任务只需1步训练的原因：静态图像没有时间信息，传统多时间步只是低效地编码数值

---

## 2025年5月中旬 - 第二篇论文：Spike2Former精读

### 完成内容
- 精读论文"Spike2Former: Efficient Spiking Transformer for High-performance Image Segmentation"（AAAI 2025 Oral）
- 理解了Mask2Former架构：Backbone → Pixel Decoder（FPN + Deformable Transformer Encoder）→ Transformer Decoder → Mask Embedding
- 理解了SNN在分割任务中崩溃的两个原因：
  - Deformable attention脉冲退化（稀疏采样×稀疏脉冲→信息消失）
  - Mask embedding信息枯竭（深层二值化后语义衰减）
- 掌握了SDTE的核心设计决策：脉冲化attention weights而非feature queries（消融实验验证：+2.5% vs -3.2%）
- 理解了SDDA的完整计算流程（Eq.4-8）：ESC生成投影权重、DWConv增强上下文、SN生成二值attention weight、Conv生成采样偏移
- 理解了NI-LIF：I-LIF + 除以D归一化，解决跨模态交互的数值尺度不匹配问题
- 认识到NI-LIF和E-SpikeFormer Eq.7（权重缩放1/D）在数学上等价
- 理解了ESC ≈ SpikeSepConv（同一实验室复用的技术模块）

### 关键认知
- Spike2Former"完全首次"的创新点：诊断并解决deformable attention中的脉冲退化
- NI-LIF的价值在于发现问题（I-LIF在复杂架构中不稳定）而非解决方案本身（除以D）
- 两篇论文的关系：E-SpikeFormer是地基（backbone），Spike2Former是上层建筑（下游任务适配）
- Mask是像素级的分割标记，mask embedding通过MLP映射query特征向量，与pixel embedding做点积生成分割结果
- Deformable的"可变形"体现在Δp_gk（网络根据输入动态预测的采样偏移量）

---

## 2025年5月中旬 - 两篇论文合并汇报

### 完成内容
- 制作了19页合并汇报PPT，包含：
  - Part 1（Slide 2-10）：E-SpikeFormer完整讲解（背景、缺陷分析、SFA公式推导、Proposition 1证明、误差分析、图解对比、架构+MIM、实验+消融）
  - Part 2（Slide 11-15）：Spike2Former完整讲解（过渡页、架构图、问题分析、SDTE公式、SDME+NI-LIF、实验结果）
  - Part 3（Slide 16-18）：技术脉络时间线、创新点辨析、总结
- 在PPT中加入了Spike2Former架构图（Figure 2）

---

## 2025年5月中旬 - 第三篇论文选定：NSLLM

### 决策过程
- 候选论文：Multiscale Spatiotemporal、Training-Free ANN2SNN、Spike2Former、NSLLM
- 最终选择NSLLM，原因：
  - 老师明确要求"SNN与大模型结合"方向
  - 同一实验室（BICLab）出品，技术脉络衔接
  - 核心神经元机制（FAI-SN）和SFA一脉相承
  - 发表在NSR（顶刊），2026年最新，有FPGA硬件验证

### 初步理解
- NSLLM的四个技术层面：FAI-SN神经元（已熟悉90%）、Spiking Linear Attention（基于RWKV）、逐层量化+spike sparsification、可解释性分析
- 阅读重点优先级：FAI-SN和sparsification（必须搞懂）> Linear Attention和量化策略（理解大意）> FPGA和可解释性（了解结论）

### 当前状态
- 已通读论文全文，理解了整体框架和各模块的定位
- 下一步：深入理解Spiking Linear Attention的工作原理、逐层量化敏感度分析的具体算法、spike sparsification如何修改膜电位分布

---

## 技术脉络总结（跨三篇论文）

```
整数/多比特脉冲减少量化误差（2022前）
    ↓
I-LIF：整数训练+脉冲推理，用于检测（SpikeYOLO, ECCV 2024）
    ↓
SFA：理论化等价性 + E-SpikeFormer架构 + SSC/MIM扩展（TPAMI 2025）
    ↓                              ↓
NI-LIF：归一化解决复杂架构       FAI-SN：扩展到十亿参数LLM
训练不稳定 + 分割任务适配         + MatMul-free + 可解释性
（Spike2Former, AAAI 2025）       （NSLLM, NSR 2026）
```
