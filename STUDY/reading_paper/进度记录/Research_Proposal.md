# Research Proposal: SNN脉冲神经网络文献阅读与研究方向探索

## 研究背景

脉冲神经网络（SNN）是一种模仿生物大脑工作方式的低功耗计算范式，通过二值脉冲（0/1）传递信息，仅在脉冲到达时触发稀疏加法运算。其核心优势是超低功耗，适合部署在神经形态芯片上。然而，SNN的精度长期落后于传统ANN，且训练成本高昂（多时间步模拟导致内存O(L×T)）。

近年来，BICLab（中科院自动化所，李国齐教授团队）通过一系列工作，系统性地推动SNN逼近ANN性能：从Spike-driven Transformer（NeurIPS 2023）到Meta-SpikeFormer（ICLR 2024），再到SpikeYOLO（ECCV 2024 Best Paper Candidate）、E-SpikeFormer（TPAMI 2025）、Spike2Former（AAAI 2025 Oral），直至NSLLM（NSR 2026）。

## 研究目标

**短期目标**：通过系统阅读BICLab的系列论文，深入理解SNN的核心技术脉络——特别是"整数训练+脉冲推理"的等价性框架及其在不同任务中的应用。

**中期目标**：掌握SNN与大语言模型（LLM）结合的技术路线（NSLLM），理解如何将LLM转化为脉冲驱动、MatMul-free的神经形态模型。

**长期目标**：探索SNN+大模型的交叉方向，寻找可能的研究切入点。

## 核心技术线

### 技术线主干：整数训练 + 脉冲推理的等价性

1. **I-LIF（SpikeYOLO, ECCV 2024）**：首次提出整数训练+脉冲推理，用于目标检测
2. **SFA（E-SpikeFormer, TPAMI 2025）**：理论化整数-脉冲等价性（Proposition 1），系统分析近似误差，结合高效架构和MIM预训练扩展到173M参数
3. **NI-LIF（Spike2Former, AAAI 2025）**：发现I-LIF在复杂架构跨模态交互中不稳定，通过归一化（除以D）解决
4. **FAI-SN（NSLLM, NSR 2026）**：将整数脉冲思想扩展到十亿参数级LLM，支持二值/浮点灵活切换

### 技术线分支

- **架构设计**：SpikeSepConv / ESC（能效可分离卷积）、SDSA / SDDA（脉冲驱动注意力变体）、Spiking Linear Attention（基于RWKV的线性复杂度注意力）
- **规模扩展**：SSC + MIM预训练（解决SNN扩展退化）、逐层量化敏感度分析（混合精度优化）
- **脉冲稀疏化**：NSFR随时间步递减（E-SpikeFormer观察）、spike sparsification修改膜电位分布（NSLLM）
- **硬件部署**：异步神经形态芯片Speck、FPGA MatMul-free硬件核
- **可解释性**：用KS熵、Shannon熵、互信息从神经科学角度分析模型

## 已读论文清单

| 序号 | 论文 | 发表 | 核心贡献 | 阅读状态 |
|------|------|------|---------|----------|
| 1 | E-SpikeFormer (Spike-driven Transformer V3) | TPAMI 2025 | SFA理论框架 + 高效架构 + SSC/MIM扩展 | ✅ 已精读 |
| 2 | Spike2Former | AAAI 2025 Oral | 解决分割架构脉冲退化 + NI-LIF | ✅ 已精读 |
| 3 | NSLLM | NSR 2026 | 十亿参数级脉冲LLM + MatMul-free + 可解释性 | 📖 阅读中 |

## 待读论文

- Spike-driven Transformer (NeurIPS 2023) — 系列起点
- Meta-SpikeFormer (ICLR 2024) — 元架构设计
- SpikeYOLO (ECCV 2024) — I-LIF首次提出
- SpikeLLM (ICLR 2025) — SNN+LLM的另一条路线，可做对比

## 老师的要求

- 研究方向：SNN与大模型结合
- 文献阅读汇报要求：了解领域背景、理解技术意义、对比其他路径的创新点和优势、理解技术细节（公式、算法）
