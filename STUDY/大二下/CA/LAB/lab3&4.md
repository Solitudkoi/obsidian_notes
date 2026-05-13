## cache.v 逐块讲解

### 第一块：模块接口

```verilog
module cache (
    input wire clk,
    input wire [31:0] addr,
    input wire load,    // CPU读，命中时更新recent
    input wire store,   // 从内存填数据进cache
    input wire edit,    // CPU写，命中时修改数据
    input wire invalid, // 让两路都失效
    input wire [2:0] u_b_h_w,
    input wire [31:0] din,
    output reg hit = 0,
    output reg [31:0] dout = 0,
    output reg valid = 0,  // LRU路的valid
    output reg dirty = 0,  // LRU路的dirty
    output reg [22:0] tag = 0  // LRU路的tag
);
```

重点：`valid`、`dirty`、`tag` 三个输出是 **LRU路**（最久未用的那路）的状态，不是命中路的状态。CMU 在 miss 时读这三个输出，判断需不需要做 dirty writeback。

---

### 第二块：内部存储

```verilog
reg [ELEMENT_NUM-1:0] inner_recent = 0; // 64位，每位对应一个element
reg [ELEMENT_NUM-1:0] inner_valid  = 0;
reg [ELEMENT_NUM-1:0] inner_dirty  = 0;
reg [TAG_BITS-1:0] inner_tag [0:ELEMENT_NUM-1]; // 64项，每项23位
reg [31:0] inner_data [0:ELEMENT_NUM*ELEMENT_WORDS-1]; // 256项，每项32位
```

`inner_recent/valid/dirty` 用**一个大位宽寄存器**存64个元素的状态，用元素编号做下标取某一位，这样比64个单独的寄存器更紧凑。`inner_data` 是 64元素×4word = 256项的一维数组。

---

### 第三块：地址分解

```verilog
assign addr_tag      = addr[31:9];          // 23位，识别是哪块内存
assign addr_index    = addr[8:4];           // 5位，选哪个set（共32个）
assign addr_element1 = {addr_index, 1'b0}; // way0的编号 = set×2+0
assign addr_element2 = {addr_index, 1'b1}; // way1的编号 = set×2+1
assign addr_word1    = {addr_element1, addr[3:2]}; // inner_data里的下标
assign addr_word2    = {addr_element2, addr[3:2]};
```

`{addr_index, 1'b0}` 等价于 `addr_index × 2 + 0`，用拼接代替乘法。`addr_word` 是8位，索引256项的 `inner_data`，等价于 `element × 4 + word_offset`。

---

### 第四块：组合逻辑取数据和状态

```verilog
// 取两路的数据
assign word1 = inner_data[addr_word1];
assign word2 = inner_data[addr_word2];

// 从32位word里切出halfword（addr[1]选高/低16位）
assign half_word1 = addr[1] ? word1[31:16] : word1[15:0];

// 从32位word里切出byte（addr[1:0]选4个字节之一）
assign byte1 = addr[1] ?
                addr[0] ? word1[31:24] : word1[23:16] :
                addr[0] ? word1[15:8]  : word1[7:0];

// 取两路的状态
assign valid1  = inner_valid[addr_element1];
assign dirty1  = inner_dirty[addr_element1];
assign tag1    = inner_tag[addr_element1];
assign recent1 = inner_recent[addr_element1];
// way2同理

// 命中判断
assign hit1 = valid1 & (tag1 == addr_tag);
assign hit2 = valid2 & (tag2 == addr_tag);
```

这一整块全是 `assign`，**没有时钟，瞬间响应**。目的是把两路的数据和状态同时取出来备用，让后面的时序逻辑直接用，不用再去查数组。`valid & tag匹配` 两个条件缺一不可：只比较tag会在冷启动时误判，只看valid不能区分不同地址。

---

### 第五块：时序逻辑——固定每拍更新的输出

```verilog
always @ (posedge clk) begin
    // 无论发生什么，每拍都更新LRU路的状态给CMU看
    valid <= recent1 ? valid2 : valid1;
    dirty <= recent1 ? dirty2 : dirty1;
    tag   <= recent1 ? tag2   : tag1;
    hit   <= hit1 | hit2;
```

`recent1=1` 说明 way0 最近被用，LRU 是 way1，所以输出 way1 的状态。这四行每拍无条件执行，CMU 随时都能读到最新的 LRU 状态。

---

### 第六块：load 操作

```verilog
if (load) begin
    if (hit1) begin
        dout <=
            u_b_h_w[1] ? word1 :   // LW：整个32位
            u_b_h_w[0] ? {u_b_h_w[2] ? 16'b0 : {16{half_word1[15]}},
                           half_word1} :   // LH/LHU：16位
                          {u_b_h_w[2] ? 24'b0 : {24{byte1[7]}},
                           byte1};         // LB/LBU：8位
        inner_recent[addr_element1] <= 1'b1; // way0标记为最近使用
        inner_recent[addr_element2] <= 1'b0;
    end
    else if (hit2) begin ... end  // 对称
end
else  // load=0：CMU做writeback用，不更新recent
    dout <= inner_data[recent1 ? addr_word2 : addr_word1];
```

`u_b_h_w[1]` 选 word/subword，`u_b_h_w[0]` 选 halfword/byte，`u_b_h_w[2]` 选有符号/无符号扩展。`{16{half_word1[15]}}` 是把符号位复制16次做符号扩展。

`load=0` 的 else 分支是给 CMU writeback 用的：不触发 recent 更新，直接输出 LRU 路当前 word 的数据，供 CMU 一个 word 一个 word 地搬回内存。

---

### 第七块：edit 操作（写命中）

```verilog
if (edit) begin
    if (hit1) begin
        inner_data[addr_word1] <=
            u_b_h_w[1] ? din :        // word：整个替换
            u_b_h_w[0] ?              // halfword
                addr[1] ? {din[15:0], word1[15:0]}   // 替换高16位
                        : {word1[31:16], din[15:0]}  // 替换低16位
            :                         // byte
                addr[1] ?
                    addr[0] ? {din[7:0], word1[23:0]}            // byte3
                            : {word1[31:24], din[7:0], word1[15:0]} // byte2
                :
                    addr[0] ? {word1[31:16], din[7:0], word1[7:0]}  // byte1
                            : {word1[31:8], din[7:0]};              // byte0
        inner_dirty[addr_element1] <= 1'b1;  // 改了，和内存不一致
        inner_recent[addr_element1] <= 1'b1;
        inner_recent[addr_element2] <= 1'b0;
    end
end
```

Verilog 不支持 `inner_data[x][7:0] <= din`，所以必须用拼接把没改的字节原样保留，只换要改的部分。dirty 置1因为 cache 里的数据比内存新了。

---

### 第八块：store 操作（填充）

```verilog
if (store) begin
    if (recent1) begin   // way0最近被用，LRU是way1，替换way1
        inner_data[addr_word2]     <= din;
        inner_valid[addr_element2] <= 1'b1;
        inner_dirty[addr_element2] <= 1'b0;  // 刚从内存来，dirty=0
        inner_tag[addr_element2]   <= addr_tag;
    end else begin       // LRU是way0，替换way0
        inner_data[addr_word1]     <= din;
        ...
    end
    // 注意：store不更新recent，填数据不算CPU访问
end
```

store 和 edit 的核心区别：edit 是 CPU 主动改数据（dirty=1），store 是 CMU 从内存搬来的（dirty=0）。store 不更新 recent 是因为这是内存操作，不是 CPU 的访问行为，不能影响 LRU 判断。

---

### 第九块：invalid 操作

```verilog
if (invalid) begin
    inner_recent[addr_element1] <= 1'b0;
    inner_recent[addr_element2] <= 1'b0;
    inner_valid[addr_element1]  <= 1'b0;
    inner_valid[addr_element2]  <= 1'b0;
    inner_dirty[addr_element1]  <= 1'b0;
    inner_dirty[addr_element2]  <= 1'b0;
end
```

把当前 set 的两路全部清零，相当于"忘掉"这个 set。本实验里 CMU 传的 `invalid` 固定是0，没有实际用到。

---

## cmu.v 逐块讲解

### 第一块：接口

```verilog
// CPU侧
input [31:0] addr_rw,  // CPU要读写的地址
input en_r,            // CPU发起读
input en_w,            // CPU发起写
input [2:0] u_b_h_w,
input [31:0] data_w,   // CPU要写的数据
output [31:0] data_r,  // 返回给CPU的数据
output stall,          // 让CPU等待

// 内存侧
output reg mem_cs_o,       // 片选，=1才能访问内存
output reg mem_we_o,       // =1写内存，=0读内存
output reg [31:0] mem_addr_o,
input [31:0] mem_data_i,   // 内存返回的数据
output [31:0] mem_data_o,  // 要写入内存的数据
input mem_ack_i,           // 内存完成一次word传输的确认
```

---

### 第二块：cache实例化

```verilog
cache CACHE (
    .clk(~clk),  // 关键：取反！cache在negedge clk工作
    ...
);
```

CMU 在 posedge 更新 state 和控制信号 → cache 在 negedge（即`~clk`的posedge）采样处理 → 下一个 posedge CMU 读到结果。一个周期内完成"CMU发命令→cache执行→CMU读结果"的完整流程，这是 hit 只需要1个周期的根本原因。

---

### 第三块：状态定义和寄存器

```verilog
localparam S_IDLE=0, S_PRE_BACK=1, S_BACK=2, S_FILL=3, S_WAIT=4;

reg [2:0] state = 0;
reg [2:0] next_state = 0;
reg [1:0] word_count = 0;      // 当前处理第几个word（0~3）
reg [1:0] next_word_count = 0;
```

两段式 FSM：`state` 是寄存器（时序），`next_state` 是组合逻辑算出来的。`word_count` 记录 BACK/FILL 时已经处理了几个 word，收到 ack 就加1，到3（`2'b11`）说明4个word全部完成。

---

### 第四块：状态寄存器更新（时序逻辑）

```verilog
always @ (posedge clk) begin
    if (rst) begin
        state      <= S_IDLE;
        word_count <= 2'b00;
    end else begin
        state      <= next_state;
        word_count <= next_word_count;
    end
end
```

每个 posedge 把组合逻辑算好的 `next_state` 和 `next_word_count` 存进寄存器。这是标准的两段式 FSM 写法，把时序和组合逻辑分开，避免产生毛刺。

---

### 第五块：状态转移（组合逻辑）

```verilog
always @ (*) begin
    case (state)
        S_IDLE: begin
            if (en_r || en_w) begin
                if (cache_hit)
                    next_state = S_IDLE;      // 命中：留在IDLE，1周期搞定
                else if (cache_valid && cache_dirty)
                    next_state = S_PRE_BACK;  // miss且LRU路脏：要先写回
                else
                    next_state = S_FILL;      // miss且LRU路干净：直接填
            end else
                next_state = S_IDLE;
            next_word_count = 2'b00;          // IDLE时计数器清零
        end

        S_PRE_BACK: begin
            next_state = S_BACK;              // 无条件跳BACK
            next_word_count = 2'b00;
        end

        S_BACK: begin
            if (mem_ack_i && word_count == 2'b11)
                next_state = S_FILL;          // 4个word写完了
            else
                next_state = S_BACK;          // 还没完，继续等ack
            next_word_count = mem_ack_i ? word_count + 1 : word_count;
        end

        S_FILL: begin                         // 和S_BACK对称
            if (mem_ack_i && word_count == 2'b11)
                next_state = S_WAIT;
            else
                next_state = S_FILL;
            next_word_count = mem_ack_i ? word_count + 1 : word_count;
        end

        S_WAIT: begin
            next_state = S_IDLE;              // 无条件回IDLE
            next_word_count = 2'b00;
        end
    endcase
end
```

`{ELEMENT_WORDS_WIDTH{1'b1}}` = `{2{1'b1}}` = `2'b11` = 3，代表第4个word（0,1,2,3）。`word_count==3` 时说明4个word全部处理完毕。

---

### 第六块：cache控制信号

```verilog
always @ (*) begin
    case(state)
        S_IDLE, S_WAIT: begin
            cache_addr    = addr_rw;   // 用CPU的地址
            cache_load    = en_r;      // CPU读 → cache load
            cache_edit    = en_w;      // CPU写 → cache edit
            cache_store   = 1'b0;
            cache_u_b_h_w = u_b_h_w;
            cache_din     = data_w;
        end

        S_BACK, S_PRE_BACK: begin
            // 高28位（tag+index）不变，word部分换成word_count，byte补0
            cache_addr = {addr_rw[31:4], word_count, 2'b00};
            cache_load = 1'b0;  // 不用load，地址对了dout自动出来
                                // load=1会更新recent，破坏LRU
            cache_edit  = 1'b0;
            cache_store = 1'b0;
            cache_u_b_h_w = 3'b010;  // word粒度
            cache_din   = 32'b0;
        end

        S_FILL: begin
            cache_addr  = {addr_rw[31:4], word_count, 2'b00};
            cache_load  = 1'b0;
            cache_edit  = 1'b0;
            cache_store = mem_ack_i;   // ack=1时才写，数据才有效
            cache_u_b_h_w = 3'b010;
            cache_din   = mem_data_i;  // 内存数据写进cache
        end
    endcase
end
assign data_r = cache_dout;
```

**S_IDLE 和 S_WAIT 用同一套逻辑**：两者都是"把CPU请求转发给cache"，S_IDLE 处理 hit，S_WAIT 处理 miss 后的重新执行，逻辑完全相同。

**S_BACK 为什么 `cache_load=0`**：cache 的 `load=0` else 分支会直接输出 LRU 路当前 word 到 `dout`，只要 `cache_addr` 对了数据就出来，不需要 `load=1`。而且 `load=1` 会刷新 recent bit，这会破坏 LRU 状态，导致后续 store 写错路。

**S_FILL 为什么 `cache_store=mem_ack_i`**：内存读需要4个周期才返回 ack，只有 `ack=1` 的那一拍数据才真正有效，才能写进 cache。如果直接写 `1'b1`，会把无效数据存进 cache。

---

### 第七块：内存控制信号

```verilog
always @ (*) begin
    case (next_state)  // 用next_state，提前一拍发请求
        S_BACK: begin
            mem_cs_o  = 1'b1;
            mem_we_o  = 1'b1;    // 写内存
            // 写回地址：用cache_tag（旧脏行的tag）重建原始地址
            mem_addr_o = {cache_tag,       // 旧tag（23位）
                          addr_rw[8:4],    // set index（5位，不变）
                          word_count,      // 第几个word（2位）
                          2'b00};          // byte offset补0
        end
        S_FILL: begin
            mem_cs_o  = 1'b1;
            mem_we_o  = 1'b0;    // 读内存
            mem_addr_o = {addr_rw[31:4], word_count, 2'b00};
        end
        default: begin   // S_IDLE, S_PRE_BACK, S_WAIT
            mem_cs_o   = 1'b0;
            mem_we_o   = 1'b0;
            mem_addr_o = 32'b0;
        end
    endcase
end
assign mem_data_o = cache_dout;
```

**为什么用 `next_state` 驱动内存**：提前一拍发出内存请求，内存在 CMU 进入下一个状态的同时就开始准备，节省一个周期。

**S_BACK 的地址为什么用 `cache_tag` 而不是 `addr_rw` 的 tag**：`addr_rw` 的 tag 是 CPU 新请求的地址（要读的新数据），`cache_tag` 是 cache 输出的 LRU 路的旧 tag（被替换的脏数据原来在内存的位置）。写回必须写到原来的内存位置，所以必须用旧 tag。

**S_PRE_BACK 时 `mem_cs=0`**：PRE_BACK 只是预读 cache，不访问内存，所以片选不使能。

---

### 第八块：stall 信号

```verilog
assign stall = ((state == S_IDLE && (en_r || en_w) && !cache_hit) ||
                (state == S_PRE_BACK || state == S_BACK || state == S_FILL))
               ? 1'b1 : 1'b0;
```

**stall=1 的两种情况**：

1. S_IDLE 有请求但 miss：还没开始处理，CPU 不能走
2. PRE_BACK/BACK/FILL：正在和内存交互，CPU 必须等

**stall=0 的情况**：

- S_IDLE + hit：当拍 cache 就处理完了（negedge执行），CPU 不用等
- S_WAIT：填充完毕重新执行，这拍结束 CPU 就有结果，放行
- S_IDLE + 无请求：没事做，不需要 stall