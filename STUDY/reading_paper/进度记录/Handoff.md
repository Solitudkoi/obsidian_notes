# Handoff: SNN文献阅读任务交接文档

## 用户背景
- 大二学生，没什么基础，正在系统学习SNN（脉冲神经网络）领域
- 老师要求研究方向：SNN与大模型结合
- 文献汇报要求：了解背景、理解技术意义、对比创新点、掌握技术细节（公式/算法）
- 用户具有批判性思维，会追问"这个创新点是不是真的首次提出"

## 已完成的工作

### 已精读论文
1. **E-SpikeFormer**（TPAMI 2025）：SFA方法、Proposition 1证明、近似误差分析、E-SpikeFormer架构、SSC+MIM预训练。已制作18页技术详细版PPT。
2. **Spike2Former**（AAAI 2025 Oral）：SDTE解决deformable attention脉冲退化、NI-LIF、SDME。已制作19页合并汇报PPT（含两篇论文）。

### 用户已掌握的知识
- SNN基础：脉冲神经元的积分/发射/重置机制（Eq.1-3）、二值脉冲的空间和时间缺陷
- SFA核心：整数训练=脉冲推理的等价性、Proposition 1的IF-SR证明、Eq.4-7的推导逻辑、近似误差三段分析
- 架构知识：SpikeSepConv/ESC、SDSA/SDDA、Mask2Former的backbone-decoder-mask embedding流程
- 创新点辨析：能区分"完全首次"vs"系统化贡献"vs"工程改进"
- 技术脉络：I-LIF → SFA → NI-LIF → FAI-SN 的演进关系

## 当前进行中的任务

### 正在阅读：NSLLM（NSR 2026）
- "Neuromorphic spike-based large language model"
- 已通读全文，理解了整体框架
- 需要深入理解的技术点：
  1. Spiking Linear Attention（基于RWKV，O(L²d)→O(Ld)，如何消除MatMul）
  2. 逐层量化敏感度分析（逐层测试选最优量化模式的算法）
  3. Spike sparsification（修改膜电位分布从钟形→半钟形，降低发射率）
  4. 可解释性框架（KS熵、Shannon熵、互信息分析各层信息处理）

### 阅读重点优先级
- 必须搞懂：FAI-SN（已基本掌握）、spike sparsification原理
- 理解大意：Spiking Linear Attention、逐层量化
- 了解结论：FPGA硬件（13.849W、161.8 tokens/s、19.8×能效提升）、可解释性（无歧义文本编码更有效）

## 下一步计划
- 深入NSLLM的技术细节讨论
- 制作NSLLM的文献汇报PPT（可能与前两篇合并或单独汇报）
- 关注BICLab后续在SNN+LLM方向的新工作

## 重要提醒
- 用户倾向于中文交流
- 用户会质疑论文自称的"novel"是否真正首次，需要如实分析
- 解释技术概念时多用类比和例子，适合没什么基础的大二学生
- 所有论文来自同一实验室BICLab（中科院自动化所，李国齐教授团队）
- 项目文件存放在 /mnt/user-data/outputs/

## 关键文件位置
- Research_Proposal.md — 研究方案和路线
- Progress.md — 完整进度日志
- 已生成的PPT文件在 /mnt/user-data/outputs/ 目录下
