> 本 CPU 实现的是 **RV32I 五级流水线处理器**，流水线阶段为  
> **IF → ID → EX → MEM → WB**。
> 
> - 数据相关通过 **forwarding + stall** 解决
>     
> - 控制相关通过 **predict-not-taken + flush** 解决
>     
> - 分支在 **ID 阶段判断**，减少 branch penalty。
>     

---

# 二、指令讲解（按 PC 顺序）

老师一般希望你 **指着程序地址解释**。  
下面是最推荐的顺序。

---

# 1 验证 Load-Use Hazard（stall）

PC：

```
0x04   lw   x2,4(x0)
0x08   lw   x4,8(x0)
0x0C   add  x1,x2,x4
0x10   addi x1,x1,-1
```

重点讲：

```
0x0C add x1,x2,x4
```

原理：

```
lw 的数据 MEM stage 才得到
add 在 EX stage 就需要
```

所以 forwarding 来不及。

CPU 会：

```
stall 1 cycle
```

波形现象：

```
PC_ID = 0x0C
PC_ID = 0x0C
```

说明 **流水线停一拍**。

---

# 2 验证 Forwarding（ALU→ALU）

PC：

```
0x20  sub x1,x4,x2
0x24  and x1,x4,x2
0x28  or  x1,x4,x2
0x2C  xor x1,x4,x2
```

这里连续使用前一条 ALU 结果。

如果没有 forwarding：

```
需要 stall
```

但现在不会 stall，因为使用：

```
EX/MEM → EX forwarding
```

波形现象：

```
ALU_Out = 8
ALU_Ain = 8
```

说明 ALU 结果直接送入下一条指令。

---

# 3 验证 Store Forwarding

PC：

```
0xF8   lw x8,24(x0)
0xFC   sw x8,28(x0)
0x100  lw x1,28(x0)
```

这里：

```
sw 使用 x8
```

但：

```
x8 来自上一条 lw
```

store 的 rs2 数据在 **MEM stage 才使用**。

因此 CPU 使用：

```
store-data forwarding
```

所以：

```
lw → sw
```

**不会 stall**。

波形现象：

```
PC_ID = 0xF8
PC_ID = 0xFC
PC_ID = 0x100
```

没有重复。

---

# 4 验证 Predict-Not-Taken（分支）

PC：

```
0x7C  beq x4,x5,label
0x80  beq x4,x4,label
```

CPU策略：

```
predict not taken
```

即：

```
PC = PC + 4
```

当 ID 阶段判断 branch 成立：

```
branch_taken = 1
```

CPU 会：

```
flush IF/ID
flush ID/EX
```

并跳转：

```
PC ← label
```

波形现象：

```
PC_IF = 0x80
PC_IF = 0x84   (预测不跳)
PC_IF = 0x8C   (实际跳转)
```

---

# 5 验证 Jump（JAL / JALR）

PC：

```
0xEC   jal  x1,12
0x128  jalr x1,0(x0)
```

波形：

```
JALR = 1
jump_PC_ID = 0x00000000
```

之后：

```
PC_IF = 0x00000000
```

说明跳转成功。

---

# 三、代码讲解部分（老师很可能问）

重点四个文件。

---

# 1 CtrlUnit.v

作用：

```
根据 opcode / funct3 / funct7
生成控制信号
```

关键代码：

```
hazard_optype
```

```verilog
parameter hazard_optype_ALU   = 2'd1;
parameter hazard_optype_LOAD  = 2'd2;
parameter hazard_optype_STORE = 2'd3;
```

作用：

把指令分类：

```
ALU
LOAD
STORE
```

供 HazardDetectionUnit 判断。

---

# 2 HazardDetectionUnit.v

这是 **老师最爱问的模块**。

---

## load-use detection

```verilog
wire load_use_A = is_load_EXE && (rd_EXE == rs1_ID) && rs1use_ID;
wire load_use_B = is_load_EXE && (rd_EXE == rs2_ID) && rs2use_ID;
```

意思：

```
EXE 是 load
下一条使用相同寄存器
```

就产生：

```
load-use hazard
```

---

## stall 实现

```verilog
assign PC_EN_IF = ~need_stall;
assign reg_FD_EN = ~need_stall;
```

stall 时：

```
PC 不更新
IF/ID 不更新
```

流水线停一拍。

---

## 为什么 store 不 stall

关键代码：

```verilog
(load_use_B && !is_store_ID)
```

意思：

```
如果 rs2 相关且是 store
```

不算 hazard。

因为：

```
store 数据可以 forwarding
```

---

## branch flush

```verilog
assign reg_FD_flush = branch_taken;
assign reg_DE_flush = branch_taken;
```

branch 在：

```
ID stage
```

判断。

错误路径在：

```
IF
ID
```

所以 flush 两级。

---

# 3 RV32core.v

作用：

```
连接五级流水线
```

流水线结构：

```
IF → ID → EX → MEM → WB
```

forwarding 来源：

```
EX/MEM
MEM/WB
```

控制信号：

```
forward_ctrl_A
forward_ctrl_B
```

---

# 4 cmp_32.v

实现 branch 比较。

支持：

```
beq
bne
blt
bge
bltu
bgeu
```

根据：

```
funct3
```

选择比较方式。

---

# 四、老师最可能问的 5 个问题

### 1 为什么 load-use 必须 stall？

因为：

```
lw 数据 MEM stage 才得到
```

forwarding 来不及。

---

### 2 为什么 store 不 stall？

因为：

```
store rs2 在 MEM stage 使用
```

可以：

```
store forwarding
```

---

### 3 forwarding 优先级？

```
MEM > EXE > register
```

---

### 4 为什么 x0 不参与 hazard？

```
x0 永远为 0
```

---

### 5 为什么 predict-not-taken？

```
实现简单
硬件开销小
```

---

# 五、最后总结一句

验收结束时可以这样总结：

> 本 CPU 实现 RV32I 五级流水线，通过 forwarding 解决大多数 RAW hazard，对 load-use hazard 插入 1-cycle stall，对 `lw→sw` 使用 store forwarding 避免 stall，控制相关采用 predict-not-taken，并在 ID 阶段判断分支并 flush 错误路径指令。




# 1 为什么 Load-Use Hazard 必须 stall？

### 代码位置

`HazardDetectionUnit.v`

```verilog
wire load_use_A = is_load_EXE && (rd_EXE == rs1_ID) && rs1use_ID;
wire load_use_B = is_load_EXE && (rd_EXE == rs2_ID) && rs2use_ID;
```

### 解释

当：

```
EXE stage = lw
ID stage = 使用该寄存器
```

就发生：

```
load-use hazard
```

原因：

```
lw 的数据 MEM stage 才得到
下一条 ALU EX stage 就需要
```

forwarding 来不及。

所以：

```
stall 1 cycle
```

---

# 2 stall 是怎么实现的？

### 代码

```verilog
assign PC_EN_IF = ~need_stall;
assign reg_FD_EN = ~need_stall;
assign reg_FD_stall = need_stall;
```

### 原理

stall 时：

```
PC 不更新
IF/ID 不更新
```

流水线停一拍。

所以 PC 会出现：

```
PC_ID = 0x0C
PC_ID = 0x0C
```

---

# 3 为什么 store 不 stall？

### 关键代码

```verilog
wire load_use_hazard =
        load_use_A ||
        (load_use_B && !is_store_ID);
```

### 解释

如果相关发生在：

```
rs1
```

必须 stall。

如果发生在：

```
rs2 且是 store
```

不 stall。

因为：

```
store rs2 在 MEM stage 才使用
```

可以：

```
store forwarding
```

---

# 4 forwarding 的优先级为什么是 MEM > EXE？

### 代码

```verilog
assign forward_ctrl_A =
        ({2{raw_A_MEM}} & 2'b11) |
        ({2{raw_A_EXE & ~raw_A_MEM}} & 2'b10);
```

### 解释

优先级：

```
MEM > EXE > register
```

原因：

```
MEM 的数据更新
```

例如：

```
add x1,x2,x3
add x1,x1,x4
add x5,x1,x6
```

第三条应该用 **第二条结果**。

---

# 5 为什么要判断 rd != 0？

### 代码

```verilog
(rd_EXE != 0)
(rd_MEM != 0)
```

### 原因

RISC-V：

```
x0 永远为 0
```

写入 x0 没意义。

因此：

```
x0 不参与 hazard
```

---

# 6 branch 为什么在 ID 阶段判断？

### 代码

```verilog
wire branch_taken = (B_valid & cmp_res_ID) | JAL_ID | JALR_ID;
```

### 解释

如果在：

```
EX stage
```

判断，

需要 flush：

```
IF
ID
EX
```

但现在在：

```
ID stage
```

只需要 flush：

```
IF
ID
```

branch penalty 更小。

---

# 7 为什么 flush 两级？

### 代码

```verilog
assign reg_FD_flush = branch_taken;
assign reg_DE_flush = branch_taken;
```

### 解释

当 branch 在 ID stage 判断：

错误路径的指令在：

```
IF
ID
```

因此需要：

```
flush IF/ID
flush ID/EX
```

---

# 8 predict-not-taken 的原理是什么？

CPU 默认：

```
PC = PC + 4
```

只有当：

```
branch_taken = 1
```

才：

```
flush
jump
```

优点：

```
实现简单
硬件成本低
```

---

# 9 forwarding 数据从哪里来？

在 `RV32core.v` 中：

```
EX/MEM pipeline register
MEM/WB pipeline register
```

ALU 输入可能来自：

```
寄存器
EX forwarding
MEM forwarding
```

控制信号：

```
forward_ctrl_A
forward_ctrl_B
```

---

# 10 cmp_32 模块是干什么的？

`cmp_32.v`

实现：

```
branch 条件判断
```

支持：

```
beq
bne
blt
bge
bltu
bgeu
```

根据：

```
funct3
```

选择比较方式。

输出：

```
cmp_res
```

供 branch 判断。

---

# 最后给你一个 **老师最喜欢问的综合问题**

### Q

为什么：

```
lw → sw
```

不 stall？

### A

因为：

```
store rs2 在 MEM stage 才使用
```

而：

```
lw MEM stage 得到数据
```

所以可以：

```
MEM → store forwarding
```

不需要 stall。

---

# 最后给你一个 **验收万能总结**

老师如果让你总结 CPU，可以说：

> 本 CPU 实现 RV32I 五级流水线结构，通过 forwarding 解决大部分 RAW hazard，对 load-use hazard 插入 1-cycle stall，对 `lw→sw` 使用 store forwarding 避免 stall，控制相关采用 predict-not-taken，并在 ID 阶段判断分支并 flush 错误路径指令。




很好，我们把 **整个验收流程按 PC 顺序完整走一遍**。  
这基本就是老师看波形时的顺序，你照这个讲 **非常顺畅，而且逻辑清晰**。

我给你做成 **“PC → 指令 → 功能 → 你应该讲什么”**。

---

# 一、PC = 0x00 开始（程序启动）

```
0x00  addi x0,x0,0
```

### 作用

NOP，用于初始化流水线。

### 可以说

> 第一条是 NOP，用来填充流水线，使流水线逐渐进入稳定状态。

---

# 二、PC = 0x04 ～ 0x10（Load + Load-Use Hazard）

```
0x04  lw   x2,4(x0)
0x08  lw   x4,8(x0)
0x0C  add  x1,x2,x4
0x10  addi x1,x1,-1
```

### 重点：0x0C

```
add x1,x2,x4
```

依赖：

```
x4 ← lw
```

### 原理

```
lw 数据 MEM stage 才得到
add EX stage 就要用
```

forwarding 来不及。

因此：

```
stall 1 cycle
```

### 波形现象

你可以指着波形说：

```
PC_ID = 0x0C
PC_ID = 0x0C
```

说明：

```
流水线停一拍
```

---

# 三、PC = 0x20 ～ 0x44（Forwarding）

```
0x20  sub x1,x4,x2
0x24  and x1,x4,x2
0x28  or  x1,x4,x2
0x2C  xor x1,x4,x2
0x30  sll x1,x4,x2
0x34  slt x1,x4,x2
0x38  slt x1,x2,x4
0x3C  srl x1,x6,x2
0x40  sra x1,x6,x2
0x44  sra x1,x7,x2
```

### 作用

验证：

```
ALU forwarding
```

### 你可以说

> 这里连续执行 ALU 指令，每条指令都会使用前一条指令的结果。如果没有 forwarding 就必须 stall，但现在通过 EX/MEM → EX forwarding 直接传递数据。

### 波形看

```
ALU_Out
ALU_Ain
```

例如：

```
ALU_Out = 8
ALU_Ain = 8
```

说明 forwarding 成功。

---

# 四、PC = 0x7C ～ 0xDC（Branch + Predict-Not-Taken）

例如：

```
0x7C  beq x4,x5,label
0x80  beq x4,x4,label
```

### CPU策略

```
predict-not-taken
```

即默认：

```
PC = PC + 4
```

### 当 branch taken

CPU：

```
flush IF/ID
flush ID/EX
```

### 波形

你可以指出：

```
PC_IF = 0x80
PC_IF = 0x84
PC_IF = 0x8C
```

解释：

```
先按顺序执行
之后跳转
```

---

# 五、PC = 0xE8 ～ 0xEC（Jump）

```
0xE8  lui x1,4
0xEC  jal x1,12
```

### 作用

测试：

```
jal
```

### 波形

```
jump_PC_ID
PC_IF
```

你可以说：

> jal 会保存返回地址，并跳转到目标 PC。

---

# 六、PC = 0xF8 ～ 0x100（Store Forwarding）

```
0xF8   lw x8,24(x0)
0xFC   sw x8,28(x0)
0x100  lw x1,28(x0)
```

### 重点

```
lw → sw
```

### 原理

store 的 rs2：

```
MEM stage 才使用
```

所以：

```
MEM → store forwarding
```

### 波形

```
PC_ID = 0xF8
PC_ID = 0xFC
PC_ID = 0x100
```

没有 stall。

---

# 七、PC = 0x128（JALR）

```
0x128  jalr x1,0(x0)
```

### 作用

测试：

```
JALR
```

### 波形

```
JALR = 1
jump_PC_ID = 0
```

之后：

```
PC_IF = 0
```

说明：

```
跳回程序开头
```

---

# 八、老师可能让你解释代码

你可以主动补一句：

> 这些功能主要在 `HazardDetectionUnit.v` 中实现，包括 load-use 检测、stall 控制、forwarding 控制以及 branch flush。

关键代码：

```
load_use_A
load_use_B
forward_ctrl_A
forward_ctrl_B
branch_taken
```

---

# 九、最后总结（强烈建议说）

最后可以这样总结：

> 本 CPU 实现 RV32I 五级流水线结构，通过 forwarding 解决大部分 RAW hazard，对 load-use hazard 插入 1-cycle stall，对 `lw→sw` 使用 store forwarding 避免 stall，控制相关采用 predict-not-taken，并在 ID 阶段判断分支并 flush 错误路径指令。

---

# 十、给你一个验收小技巧

老师如果问：

```
为什么 lw → sw 不 stall？
```

直接回答这句就够：

> 因为 store 的 rs2 在 MEM stage 才使用，而 lw 在 MEM stage 就已经得到数据，因此可以通过 MEM → store forwarding 解决，不需要 stall。




好，下面这份就是你要的 **“HazardDetectionUnit 逐段解释稿”**。你可以直接拿去准备验收。  
我按 **代码块 → 作用 → 老师可能追问 → 你怎么答** 的方式讲。

---

## 1. 模块接口

```verilog
module HazardDetectionUnit(
    input clk,
    input Branch_ID, rs1use_ID, rs2use_ID, cmp_res_ID, JALR_ID, JAL_ID,
    input[1:0] hazard_optype_ID,
    input[4:0] rd_EXE, rd_MEM, rs1_ID, rs2_ID, rs2_EXE,
    input[31:0] inst_EXE,
    output PC_EN_IF, reg_FD_EN, reg_FD_stall, reg_FD_flush,
        reg_DE_EN, reg_DE_flush, reg_EM_EN, reg_EM_flush, reg_MW_EN,
    output forward_ctrl_ls,
    output[1:0] forward_ctrl_A, forward_ctrl_B
);
```

### 你可以这样讲

这个模块专门负责 **流水线冲突处理**，主要做四件事：

- 检测 **load-use hazard**
    
- 产生 **stall**
    
- 产生 **forwarding 控制**
    
- 处理 **branch/jal/jalr 导致的 flush**
    

PPT 里实验目标就明确要求了 **Pipeline Forwarding Detection** 和 **1-cycle stall of Predict-not-taken branch design**。

### 老师可能问

为什么输入里既有 `rd_EXE` 又有 `rd_MEM`？

### 你答

因为 forwarding 可能来自两个阶段：

- EXE 前一级结果
    
- MEM 更后一级结果
    

所以要分别比较。

---

## 2. 判断 EXE 阶段是不是 Load

```verilog
wire is_load_EXE = (inst_EXE[6:0] == 7'b0000011) && (rd_EXE != 0);
```

### 作用

判断 EXE 阶段那条指令是不是 load。

### 原理

RISC-V 中 load 指令 opcode 是 `0000011`。  
加上 `rd_EXE != 0` 是为了排除写 `x0` 的情况，因为 `x0` 恒为 0，不需要参与 hazard 判断。

### 你可以这样讲

只有前一条是 load，才可能出现经典的 **load-use hazard**。PPT 里的前几条验证程序就是：

- `0x04 lw x2,4(x0)`
    
- `0x08 lw x4,8(x0)`
    
- `0x0C add x1,x2,x4`
    

这组指令就是专门用来验证 data hazard 和 stall 的。

---

## 3. 判断 ID 阶段是不是 Store

```verilog
wire is_store_ID = (hazard_optype_ID == 2'd3);
```

### 作用

判断当前 ID 阶段指令是否是 store。

### 原理

你在 CtrlUnit 里已经把指令分成三类：

- `ALU = 2'd1`
    
- `LOAD = 2'd2`
    
- `STORE = 2'd3`
    

这样 HazardDetectionUnit 不需要重新解码 opcode，只要看 `hazard_optype_ID` 就能知道当前指令类型。这个分类方式也和 PPT 里的 Hazard Detection Unit 思路一致。

### 老师可能问

为什么不直接在这个模块里重新判断是不是 store？

### 你答

因为 CtrlUnit 已经完成了指令分类，这里直接复用控制器输出，结构更清晰，也避免重复解码。

---

## 4. 检测 rs1 和 rs2 的 load-use 相关

```verilog
wire load_use_A = is_load_EXE && (rd_EXE == rs1_ID) && rs1use_ID;
wire load_use_B = is_load_EXE && (rd_EXE == rs2_ID) && rs2use_ID;
```

### 作用

分别判断：

- EXE 阶段 load 的目的寄存器，是否等于 ID 阶段当前指令的 `rs1`
    
- EXE 阶段 load 的目的寄存器，是否等于 ID 阶段当前指令的 `rs2`
    

### 原理

如果前一条 load 还没把结果写回，而后一条已经要读这个寄存器，就有相关。

`rs1use_ID / rs2use_ID` 的作用是避免误判，因为不是所有指令都会真的使用两个源寄存器。

### 你可以这样讲

这两根线分别表示：

- A 口有 load-use 相关
    
- B 口有 load-use 相关
    

---

## 5. 关键逻辑：Store 的 rs2 不作为 stall 条件

```verilog
wire load_use_hazard = load_use_A || (load_use_B && !is_store_ID);
```

### 这是最重要的一句

### 原理

这句的意思是：

- `rs1` 相关：算真正 hazard，要 stall
    
- `rs2` 相关：只有当前不是 store 时，才 stall
    
- 如果当前是 store，`rs2` 相关不 stall
    

### 为什么这么做

因为 store 的两个源寄存器用途不一样：

- `rs1`：算地址，EX 阶段就要用
    
- `rs2`：写入内存的数据，通常到更后面才真正使用
    

所以对于：

- `lw -> add`：必须 stall
    
- `lw -> sw(rs2)`：可以交给 **store-data forwarding**，不 stall
    

PPT 专门有一页 **SW After LW**，就是在验证这个场景。验证程序中对应的地址是：

- `0xF8 lw x8,24(x0)`
    
- `0xFC sw x8,28(x0)`
    
- `0x100 lw x1,28(x0)`
    

### 老师可能问

为什么 `lw -> sw` 不需要 stall？

### 你答

因为 store 的 `rs2` 不是在 EX 立刻用，而是在后面的写内存通路使用，所以可以通过 store forwarding 解决。PPT 也单独拿 `SW After LW` 做了验证。

---

## 6. 分支是否跳转

```verilog
wire B_valid = ~JALR_ID & ~JAL_ID & Branch_ID;
wire branch_taken = (B_valid & cmp_res_ID) | JAL_ID | JALR_ID;
```

### 作用

统一生成“当前这条控制流指令是否会跳”的判断。

### 逻辑拆开看

`B_valid` 表示当前是 **条件分支**，不是 jal/jalr。  
`branch_taken` 表示：

- 条件分支：比较成立才跳
    
- jal：无条件跳
    
- jalr：无条件跳
    

### 你可以这样讲

这个设计和 RV32core 的 jump_condition 保持一致：

- `branch` 由 `cmp_res_ID` 决定
    
- `jal / jalr` 直接 taken
    

PPT 中控制相关要求的是 **predict-not-taken**，并且把 branch 判断提前到 ID 阶段，这样 miss 时只需要 1-cycle 代价。

---

## 7. 检测普通 RAW hazard，用于 forwarding

```verilog
wire raw_A_EXE = (rd_EXE == rs1_ID) & rs1use_ID & (rd_EXE != 0);
wire raw_B_EXE = (rd_EXE == rs2_ID) & rs2use_ID & (rd_EXE != 0);
wire raw_A_MEM = (rd_MEM == rs1_ID) & rs1use_ID & (rd_MEM != 0);
wire raw_B_MEM = (rd_MEM == rs2_ID) & rs2use_ID & (rd_MEM != 0);
```

### 作用

判断当前 ID 阶段的源寄存器，是否和 EXE / MEM 阶段还没写回的目标寄存器相同。

### 原理

这是最常见的 RAW hazard 检测，用来决定是否需要前递。

### 老师可能问

为什么要同时比较 EXE 和 MEM？

### 你答

因为 forwarding 的来源可能不止一个阶段：

- 紧前一条指令在 EXE
    
- 再前一条指令在 MEM
    

需要同时比较，再按优先级选择。

---

## 8. Store forwarding 检测

```verilog
wire store_forward = (rd_MEM == rs2_EXE) && (rd_MEM != 0);
```

### 作用

判断当前 EXE 阶段的 store，是否要从 MEM 阶段前递 store data。

### 原理

如果 MEM 阶段那条指令即将产生的结果，正好就是当前 store 要写入内存的数据，就打开 store-data forwarding。

### 你可以这样讲

这个信号和前面的 `load_use_hazard` 配套使用：

- 前面决定 `lw -> sw(rs2)` 不 stall
    
- 这里负责真正把数据转过来
    

### 老师可能问

为什么这里没加 `RegWrite_MEM`？

### 你答

这是个可以继续优化的点。当前实验代码里用 `rd_MEM != 0` 已经能避开大多数误判，功能上是能通过验证程序的；更严格的写法可以再加 `RegWrite_MEM`。

---

## 9. ALU A 口 forwarding 控制

```verilog
assign forward_ctrl_A = ({2{raw_A_MEM}} & 2'b11) |
                        ({2{raw_A_EXE & ~raw_A_MEM & ~load_use_A}} & 2'b10);
```

### 作用

控制 ALU A 输入从哪里来。

### 优先级

这里隐含的优先级是：

- MEM forwarding 优先
    
- 再看 EXE forwarding
    
- 否则来自寄存器堆
    

### 为什么要 `~load_use_A`

因为如果这是 load-use hazard，就不能假装 EXE forwarding 能解决。  
load 的值在 EXE 级根本还没准备好，这种情况必须 stall。

### 老师可能问

为什么 MEM 优先于 EXE？

### 你答

因为如果两个阶段都命中，说明更新、更近的结果在 MEM，那就应该使用更新的值。

---

## 10. ALU B 口 forwarding 控制

```verilog
assign forward_ctrl_B = ({2{raw_B_MEM}} & 2'b11) |
                        ({2{raw_B_EXE & ~raw_B_MEM & ~load_use_B}} & 2'b10);
```

### 作用

和 A 口同理，只不过这是给 ALU 的 B 口。

### 你可以这样讲

A/B 两个口是并行处理的：

- A 口看 `rs1`
    
- B 口看 `rs2`
    

逻辑结构一样。

---

## 11. store 数据 forwarding 输出

```verilog
assign forward_ctrl_ls = store_forward;
```

### 作用

把 store_forward 直接送到 datapath，用来控制 store 数据旁路。

### 你可以这样讲

这个就是 `lw -> sw` 能不 stall 的关键控制信号。

---

## 12. 是否需要 stall

```verilog
wire need_stall = load_use_hazard;
```

### 作用

最终决定当前是否冻结流水线。

### 原理

你这里的 stall 只针对 **真正的 load-use hazard**。  
这和实验要求是一致的：PPT 强调的是 **Pipeline Forwarding Detection** 加上 **1-cycle stall**。

---

## 13. Stall 时冻结 PC 和 IF/ID

```verilog
assign PC_EN_IF = ~need_stall;
assign reg_FD_EN = ~need_stall;
assign reg_FD_stall = need_stall;
```

### 作用

当 `need_stall = 1` 时：

- PC 不更新
    
- IF/ID 寄存器不更新
    

### 效果

同一条指令会在 ID 多停一个周期。

### 你可以这样举例

PPT 和你的 log 里，最前面的这组：

- `0x04 lw`
    
- `0x08 lw`
    
- `0x0C add`
    

其中 `0x0C add` 就会在 ID 连续停一拍。

---

## 14. 分支 flush：清掉错误路径上的指令

```verilog
assign reg_FD_flush = branch_taken;
assign reg_DE_EN = 1;
assign reg_DE_flush = branch_taken;
```

### 作用

当 branch/jal/jalr 被判定为 taken 时，清掉错误路径上的前级指令。

### 为什么 flush 两级

因为你的 branch 判断在 **ID 阶段** 完成。  
这时错误路径上的年轻指令主要在：

- IF/ID
    
- ID/EX
    

所以 flush 这两级就够了。

### 你可以这样讲

这是你实现 **predict-not-taken** 的核心：

- 默认先按 `PC+4` 走
    
- 如果 ID 发现 actually taken
    
- 就 flush 错路并改 PC
    

PPT 中控制相关验证程序地址从 `0x7C` 到 `0xDC`，就是专门用来检查这套逻辑的。

---

## 15. 为什么 `reg_EM_flush = 1'b0`

```verilog
assign reg_EM_EN = 1;
assign reg_EM_flush = 1'b0;
```

### 这是你修正后的关键点

### 原理

EX/MEM 里的指令通常已经是比当前 branch 更老的正确指令，不应该被 flush。

### 你可以这样答

如果 branch 在 ID 判定，就只需要清除“后面错路上的年轻指令”，不应该把更老的 EX/MEM 指令也清掉，所以这里固定为 0 更合理。

---

## 16. MEM/WB 始终使能

```verilog
assign reg_MW_EN = 1;
```

### 作用

MEM/WB 正常推进，不需要额外冻结。

### 原理

你的 stall 主要冻结的是前端，不是后端。

---

# 你最后可以怎么总结这个模块

可以直接对老师说：

> HazardDetectionUnit 的核心思路是：  
> 对普通 RAW hazard 通过 EX/MEM、MEM/WB forwarding 解决；  
> 对 load-use hazard，由于 load 数据到 MEM 阶段才有效，因此插入 1-cycle stall；  
> 对 `lw -> sw(rs2)` 这种情况，不把它当作 stall 条件，而是通过 store-data forwarding 处理；  
> 控制相关采用 predict-not-taken，branch 在 ID 阶段判定，taken 时 flush IF/ID 和 ID/EX。

---

# 最后给你一份“老师最可能追问的 5 句快答”

**1. 为什么 `lw -> add` 要 stall？**  
因为 load 数据 MEM 才出来，下一条 ALU EX 就要用，forwarding 来不及。

**2. 为什么 `lw -> sw` 不 stall？**  
因为 store 的 `rs2` 在后面才真正使用，可以做 store-data forwarding。

**3. 为什么 branch 只 flush 两级？**  
因为 branch 在 ID 判定，只需要清掉 IF/ID 和 ID/EX 的错误路径指令。

**4. 为什么不 flush EX/MEM？**  
因为 EX/MEM 中通常是更老的正确指令，不该被误杀。

**5. 为什么要有 `hazard_optype_ID`？**  
因为 ALU、LOAD、STORE 的 hazard 处理策略不同，需要先分类。



很好，我们继续把 **CtrlUnit.v 也做成同样的“逐段解释 + 老师提问 + 标准回答”版本**。  
这是老师 **第二爱问的模块**（第一是 HazardDetectionUnit）。

CtrlUnit 主要任务只有一句话：

> **根据 opcode / funct3 / funct7 产生控制信号。**

---

# 一、CtrlUnit 模块整体作用

你可以先这样讲：

> CtrlUnit 负责对指令进行译码，根据 opcode、funct3、funct7 生成 datapath 的控制信号，包括 ALU 操作、寄存器写使能、访存控制以及 hazard 类型分类。

---

# 二、指令类型判断

CtrlUnit 通常会先判断指令类型，例如：

```verilog
R_valid
I_valid
L_valid
S_valid
B_valid
J_valid
```

这些变量通常来自：

```
opcode
```

例如：

|类型|opcode|
|---|---|
|R-type|0110011|
|I-type|0010011|
|Load|0000011|
|Store|0100011|
|Branch|1100011|
|JAL|1101111|
|JALR|1100111|

---

## 老师可能问

**为什么要先判断指令类型？**

### 你回答

因为不同类型指令需要的控制信号完全不同，例如：

- R-type：ALU + 写寄存器
    
- Load：ALU算地址 + 读内存 + 写寄存器
    
- Store：ALU算地址 + 写内存
    
- Branch：比较 + PC跳转
    

所以先分类再生成控制信号更清晰。

---

# 三、hazard_optype 生成

你代码里应该有类似：

```verilog
parameter hazard_optype_ALU   = 2'd1;
parameter hazard_optype_LOAD  = 2'd2;
parameter hazard_optype_STORE = 2'd3;
```

然后：

```verilog
assign hazard_optype =
        {2{R_valid | I_valid}} & hazard_optype_ALU  |
        {2{L_valid}}           & hazard_optype_LOAD |
        {2{S_valid}}           & hazard_optype_STORE;
```

---

## 作用

把指令分成三类：

|类型|hazard_optype|
|---|---|
|ALU|1|
|LOAD|2|
|STORE|3|

这样 HazardDetectionUnit 就能快速判断 hazard 类型。

---

## 老师可能问

**为什么 CtrlUnit 要输出 hazard_optype？**

### 你回答

因为 HazardDetectionUnit 需要知道当前指令类型，从而判断 hazard 处理方式：

- ALU：forwarding
    
- LOAD：可能 stall
    
- STORE：rs2 可以 store forwarding
    

这样 HazardDetectionUnit 就不用重新解码 opcode。

---

# 四、寄存器读写控制

CtrlUnit 一般会产生：

```
RegWrite
rs1use
rs2use
```

---

## rs1use / rs2use

这些信号用来告诉 HazardDetectionUnit：

```
当前指令是否真的使用 rs1 / rs2
```

例如：

|指令|rs1|rs2|
|---|---|---|
|addi|✓|✗|
|add|✓|✓|
|lw|✓|✗|
|sw|✓|✓|
|jal|✗|✗|

---

## 老师可能问

**为什么要有 rs1use / rs2use？**

### 你回答

为了避免误判 hazard，例如：

```
addi x1,x2,5
```

没有 rs2，如果不加 rs2use：

```
rd_EXE == rs2_ID
```

可能误判 RAW hazard。

---

# 五、ALU 操作控制

CtrlUnit 还会生成：

```
ALUop
```

通常根据：

```
opcode
funct3
funct7
```

决定 ALU 操作。

例如：

|指令|ALU操作|
|---|---|
|add|ADD|
|sub|SUB|
|and|AND|
|or|OR|
|xor|XOR|
|sll|SLL|
|srl|SRL|
|sra|SRA|

---

## 老师可能问

**为什么 sub 要看 funct7？**

### 你回答

因为：

```
add 和 sub opcode 相同
```

区别在：

```
funct7
```

```
0000000 → add
0100000 → sub
```

---

# 六、Branch 控制信号

CtrlUnit 会产生：

```
Branch
```

用于告诉 datapath：

```
当前指令是 branch
```

具体跳不跳由：

```
cmp_res
```

决定。

---

## 老师可能问

**为什么 branch 判断在 ID 阶段？**

### 你回答

因为这样可以减少 branch penalty。

如果在 EX 判断：

```
需要 flush 3级
```

在 ID 判断：

```
只需要 flush 2级
```

---

# 七、Load / Store 控制

CtrlUnit 还会产生：

```
MemRead
MemWrite
MemtoReg
```

|信号|作用|
|---|---|
|MemRead|读内存|
|MemWrite|写内存|
|MemtoReg|写回来自内存|

---

# 八、老师非常可能问的问题

### Q1

为什么 store 不写寄存器？

A：

```
store 只写 memory
```

所以：

```
RegWrite = 0
```

---

### Q2

为什么 jal 要写寄存器？

A：

```
jal 需要保存返回地址
```

即：

```
rd = PC+4
```

---

### Q3

为什么 jalr 也要写寄存器？

A：

原因相同：

```
保存返回地址
```

---

# 九、CtrlUnit 和 HazardDetectionUnit 的关系

你可以这样总结：

```
CtrlUnit
    ↓
生成 hazard_optype
    ↓
HazardDetectionUnit
    ↓
决定
stall / forwarding / flush
```

---

# 十、老师如果让你总结 CtrlUnit

可以说：

> CtrlUnit 负责根据 opcode、funct3、funct7 对指令进行译码，生成 ALU 控制、寄存器写控制、访存控制以及 hazard_optype。hazard_optype 用于帮助 HazardDetectionUnit 判断不同类型指令的数据相关处理方式。

---

# 最后给你一个非常重要的提醒

老师最可能问的其实是这一句代码：

```verilog
assign hazard_optype =
        {2{R_valid | I_valid}} & hazard_optype_ALU  |
        {2{L_valid}}           & hazard_optype_LOAD |
        {2{S_valid}}           & hazard_optype_STORE;
```

一定要记住这句话怎么解释：

> 这里使用按位与和复制运算符，把指令类型映射成 hazard 类型编码。





很好，我们把 **RV32core.v** 也整理成 **验收问答版 + 结构讲解版**。  
这个模块是 **CPU 的顶层结构**，老师一般会问 **流水线结构、forwarding 数据路径、PC 更新逻辑**。

我按 **结构 → 代码 → 老师可能问 → 标准回答** 来讲。

---

# 一、RV32core.v 的整体作用

你可以先这样说：

> RV32core 是 CPU 的顶层模块，负责连接五级流水线各个阶段，并实例化寄存器堆、ALU、HazardDetectionUnit、CtrlUnit、cmp_32 等模块，同时管理 PC 更新、forwarding 数据路径和流水线寄存器。

流水线结构：

```
IF  →  ID  →  EX  →  MEM  →  WB
```

每一级通过 **pipeline register** 连接。

---

# 二、PC 更新逻辑（老师非常爱问）

PC 更新通常类似：

```verilog
if(branch_taken)
    PC = jump_PC;
else
    PC = PC + 4;
```

### 你可以这样解释

PC 有两种更新方式：

1️⃣ **顺序执行**

```
PC = PC + 4
```

2️⃣ **跳转**

```
PC = jump_PC_ID
```

jump_PC_ID 可能来自：

```
branch
jal
jalr
```

---

## 老师可能问

### Q

为什么 branch 在 ID 阶段决定？

### A

因为这样可以减少 branch penalty。

如果在 EX 阶段判断：

```
需要 flush 3级
```

如果在 ID 判断：

```
只需要 flush 2级
```

---

# 三、IF 阶段（取指）

IF 阶段主要做：

```
PC → instruction memory → inst_IF
```

通常代码：

```verilog
inst_IF = instruction_memory[PC_IF];
```

### 作用

从指令存储器取指令。

---

## 老师可能问

为什么 PC 每次加 4？

### 回答

因为：

```
RV32I 指令长度固定 32 bit
```

也就是：

```
4 bytes
```

---

# 四、ID 阶段（译码）

ID 阶段主要做：

```
指令译码
读取寄存器
branch 判断
```

通常会实例化：

```
CtrlUnit
cmp_32
RegisterFile
```

---

## cmp_32 的作用

用于 branch 比较：

```
beq
bne
blt
bge
bltu
bgeu
```

输入：

```
rs1
rs2
funct3
```

输出：

```
cmp_res
```

---

## 老师可能问

为什么 branch 需要 cmp_32？

### 回答

因为不同 branch 指令比较方式不同，例如：

```
beq  → ==
bne  → !=
blt  → <
bge  → ≥
```

cmp_32 统一实现这些比较逻辑。

---

# 五、EX 阶段（执行）

EX 阶段做：

```
ALU 运算
地址计算
```

输入：

```
ALU_Ain
ALU_Bin
```

来源可能是：

```
寄存器
forwarding
立即数
```

---

## forwarding 数据路径

ALU 输入来自：

```
register
EX/MEM forwarding
MEM/WB forwarding
```

控制信号：

```
forward_ctrl_A
forward_ctrl_B
```

---

## 老师可能问

为什么需要 forwarding？

### 回答

因为流水线中：

```
结果还没写回寄存器
下一条指令就要用
```

forwarding 可以：

```
直接从流水线寄存器传递数据
```

避免 stall。

---

# 六、MEM 阶段（访存）

MEM 阶段主要做：

```
load
store
```

如果是 load：

```
读取 data memory
```

如果是 store：

```
写入 data memory
```

---

## store forwarding

你代码里有：

```
forward_ctrl_ls
```

作用：

```
lw → sw
```

时进行 store-data forwarding。

例如：

```
lw x8,24(x0)
sw x8,28(x0)
```

不会 stall。

---

# 七、WB 阶段（写回）

WB 阶段负责：

```
写回寄存器堆
```

写回数据来源：

```
ALU result
memory data
PC+4 (jal)
```

信号：

```
wt_data_WB
```

---

# 八、流水线寄存器

五级流水线需要 **四组寄存器**：

|寄存器|作用|
|---|---|
|IF/ID|IF → ID|
|ID/EX|ID → EX|
|EX/MEM|EX → MEM|
|MEM/WB|MEM → WB|

---

## 老师可能问

为什么需要流水线寄存器？

### 回答

因为不同阶段必须：

```
同时执行不同指令
```

流水线寄存器用于：

```
保存阶段之间的数据
```

---

# 九、stall 对流水线寄存器的影响

HazardDetectionUnit 控制：

```
PC_EN_IF
reg_FD_EN
reg_DE_flush
```

stall 时：

```
PC 不更新
IF/ID 不更新
```

flush 时：

```
ID/EX 清空
```

---

# 十、老师最喜欢问的结构问题

### 1 为什么 x0 不参与 hazard？

因为：

```
x0 永远为 0
```

写入没有意义。

---

### 2 forwarding 的优先级？

```
MEM > EXE > register
```

---

### 3 为什么 lw→add 需要 stall？

因为：

```
lw 数据 MEM stage 才得到
add EX stage 就需要
```

---

### 4 为什么 lw→sw 不 stall？

因为：

```
store rs2 在 MEM stage 使用
```

可以 store forwarding。

---

### 5 predict-not-taken 的优点？

```
实现简单
硬件开销小
```

---

# 十一、你可以用这句话总结 RV32core

如果老师让你总结 CPU：

> RV32core 实现了 RV32I 五级流水线结构，通过 CtrlUnit 完成指令译码，通过 HazardDetectionUnit 处理数据相关和控制相关，通过 forwarding 解决大多数 RAW hazard，对 load-use hazard 插入 1-cycle stall，并通过 predict-not-taken 策略处理 branch。

---

# 十二、最后给你一个验收技巧

老师如果问：

```
整个 CPU 的关键模块是哪几个？
```

你可以回答：

```
CtrlUnit
HazardDetectionUnit
RegisterFile
ALU
cmp_32
```

其中：

```
HazardDetectionUnit 是流水线核心
```




很好 👍 我给你整理一份 **“CPU流水线实验验收速记表（1页版）”**。  
你验收前 **看 2-3 分钟就能把关键点全部回忆起来**。  
内容包括 **PC地址 + 指令作用 + 代码位置 + 老师可能问的问题**。

---

# 一、CPU结构总览（开场必说）

**五级流水线：**

```
IF → ID → EX → MEM → WB
```

|阶段|作用|
|---|---|
|IF|取指|
|ID|译码 + 读寄存器 + branch判断|
|EX|ALU运算|
|MEM|访存|
|WB|写回寄存器|

关键模块：

```
CtrlUnit
HazardDetectionUnit
ALU
RegisterFile
cmp_32
```

---

# 二、数据相关验证（Forwarding）

### PC = 0x20 ～ 0x44

```
0x20 sub x1,x4,x2
0x24 and x1,x4,x2
0x28 or  x1,x4,x2
0x2C xor x1,x4,x2
```

作用：

```
验证 ALU forwarding
```

解释：

```
EX/MEM → EX forwarding
MEM/WB → EX forwarding
```

看波形：

```
ALU_Ain
ALU_Bin
ALU_Out
```

---

# 三、Load-Use Hazard（stall）

### PC = 0x04 ～ 0x10

```
0x04 lw x2,4(x0)
0x08 lw x4,8(x0)
0x0C add x1,x2,x4
0x10 addi x1,x1,-1
```

关键指令：

```
0x0C add
```

原理：

```
lw 数据 MEM 才得到
add EX 就要用
```

所以：

```
stall 1 cycle
```

波形：

```
PC_ID = 0x0C
PC_ID = 0x0C
```

---

# 四、Store Forwarding（lw→sw）

### PC = 0xF8 ～ 0x100

```
0xF8   lw x8,24(x0)
0xFC   sw x8,28(x0)
0x100  lw x1,28(x0)
```

说明：

```
lw → sw 不 stall
```

原因：

```
store rs2 在 MEM stage 才使用
```

解决方式：

```
store-data forwarding
```

波形：

```
PC_ID = F8
PC_ID = FC
PC_ID = 100
```

---

# 五、Branch验证（Predict-Not-Taken）

### PC = 0x7C ～ 0xDC

例如：

```
0x7C beq x4,x5,label
0x80 beq x4,x4,label
```

CPU策略：

```
predict not taken
```

默认：

```
PC = PC + 4
```

如果 branch taken：

```
flush IF/ID
flush ID/EX
```

波形：

```
PC_IF = 80
PC_IF = 84
PC_IF = 8C
```

---

# 六、Jump验证

```
0xEC  jal
0x128 jalr
```

作用：

```
测试 jump_PC_ID
```

波形：

```
JALR = 1
jump_PC_ID
PC_IF
```

---

# 七、HazardDetectionUnit关键代码（必记）

### load-use检测

```
load_use_A
load_use_B
```

核心：

```
is_load_EXE
rd_EXE == rs_ID
```

---

### stall控制

```
PC_EN_IF = ~need_stall
reg_FD_EN = ~need_stall
```

效果：

```
PC不更新
IF/ID不更新
```

---

### store不stall

关键代码：

```
(load_use_B && !is_store_ID)
```

解释：

```
lw → sw(rs2)
用 store forwarding
```

---

### forwarding控制

```
forward_ctrl_A
forward_ctrl_B
```

优先级：

```
MEM > EXE > register
```

---

### branch flush

```
reg_FD_flush
reg_DE_flush
```

原因：

```
branch 在 ID 判断
```

---

# 八、CtrlUnit关键点

指令分类：

```
ALU
LOAD
STORE
```

代码：

```
hazard_optype
```

```
ALU = 1
LOAD = 2
STORE = 3
```

作用：

```
帮助HazardDetectionUnit判断hazard类型
```

---

# 九、RV32core关键点

PC更新：

```
branch_taken ? jump_PC : PC+4
```

forwarding来源：

```
EX/MEM
MEM/WB
```

ALU输入：

```
ALU_Ain
ALU_Bin
```

---

# 十、cmp_32作用

实现branch比较：

```
beq
bne
blt
bge
bltu
bgeu
```

输出：

```
cmp_res
```

---

# 十一、老师最可能问的5个问题

### 1

为什么 lw→add 要 stall？

答：

```
lw数据MEM阶段才得到
forwarding来不及
```

---

### 2

为什么 lw→sw 不 stall？

答：

```
store rs2 MEM阶段才使用
可以store forwarding
```

---

### 3

forwarding优先级？

答：

```
MEM > EXE > register
```

---

### 4

为什么 x0 不参与hazard？

答：

```
x0永远为0
```

---

### 5

为什么 branch 在 ID 判断？

答：

```
减少 branch penalty
```

---

# 十二、验收最后总结（建议背下来）

> 本 CPU 实现 RV32I 五级流水线结构，通过 forwarding 解决大多数 RAW hazard，对 load-use hazard 插入 1-cycle stall，对 lw→sw 使用 store forwarding 避免 stall，控制相关采用 predict-not-taken，并在 ID 阶段判断 branch 并 flush 错误路径指令。

---

如果你愿意，我可以再给你做一份 **“老师最喜欢挖坑的 8 个问题（很多同学会答错）”**，提前准备会非常稳。