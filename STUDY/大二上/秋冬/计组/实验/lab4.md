```
// Copyright 1986-2017 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2017.4 (win64) Build 2086221 Fri Dec 15 20:55:39 MST 2017
// Date        : Tue Mar  5 10:53:44 2024
// Host        : LAPTOP-6G31RL0V running 64-bit major release  (build 9200)
// Command     : write_verilog -mode synth_stub E:/FPGA/ip/DataPath.v
// Design      : DataPath
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7a100tcsg324-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
module DataPath(clk, rst, inst_field, Data_in, ALU_Control, ImmSel, 
  MemtoReg, ALUSrc_B, Jump, Branch, RegWrite, PC_out, Data_out, ALU_out)
/* synthesis syn_black_box black_box_pad_pin="clk,rst,inst_field[31:0],Data_in[31:0],ALU_Control[2:0],ImmSel[1:0],MemtoReg[1:0],ALUSrc_B,Jump,Branch,RegWrite,PC_out[31:0],Data_out[31:0],ALU_out[31:0]" */;
  input clk;
  input rst;
  input [31:0]inst_field;
  input [31:0]Data_in;
  input [2:0]ALU_Control;
  input [1:0]ImmSel;
  input [1:0]MemtoReg;
  input ALUSrc_B;
  input Jump;
  input Branch;
  input RegWrite;
  output [31:0]PC_out;
  output [31:0]Data_out;
  output [31:0]ALU_out;
endmodule
```



```
module SCPU(
    input clk,
    input rst,
    input MIO_ready,
    input [31:0] inst_in,
    input [31:0] Data_in,
    
    output CPU_MIO,
    output MemRW,
    output [31:0] PC_out,
    output [31:0] Data_out,
    output [31:0] Addr_out
    );
```



```
memory_initialization_radix=16;
memory_initialization_vector=
00100093,00102133,002101B3,00218233,003202B3,00428333,
005303B3,00638433,007404B3,00848533,009505B3,00A58633,
00B606B3,00C68733,00D707B3,00E78833,00F808B3,01088933,
011909B3,01298A33,013A0AB3,014A8B33,015B0BB3,016B8C33,
017C0CB3,018C8D33,019D0DB3,01AD8E33,01BE0EB3,01CE8F33,
01DF0FB3,F80002E3;
```




```
|PC|指令 HEX|指令（汇编）|执行结果（寄存器与内存变化）|
|---|---|---|---|
|00|**00500093**|addi x1, x0, 5|x1 = 5|
|04|**00300113**|addi x2, x0, 3|x2 = 3|
|08|**002081B3**|add x3, x1, x2|x3 = 5 + 3 = **8**|
|0C|**40208233**|sub x4, x1, x2|x4 = 5 - 3 = **2**|
|10|**0030E2B3**|and x5, x1, x3|5 & 8 = 0 → x5 = **0**|
|14|**0030F333**|or x6, x1, x3|5\|8 = 13 → x6 = **13**|
|18|**00A0F3B3**|xor x7, x1, x10|x10=0 → x7 = **5**|
|1C|**0020F433**|slt x8, x1, x2|5<3 = false → x8 = **0**|
|20|**00A0E4B3**|andi x9, x1, 10|5 & 10 = 0 → x9 = **0**|
|24|**00A0E533**|ori x10, x1, 10|5\|10 = 15 → x10 = **15**|
|28|**00F0E5B3**|xori x11, x1, 15|5^15 = 10 → x11 = **10**|
|2C|**00A0E633**|slti x12, x1, 10|5<10 → x12 = **1**|
|30|**06400793**|addi x15, x0, 100|x15 = 100|
|34|**00112023**|sw x1, 0(x2)|MEM[x2=3] = **5**|
|38|**00012183**|lw x3, 0(x2)|x3 = MEM[3] = **5**|
|3C|**00700193**|addi x3, x0, 7|x3 = **7**|
|40|**00700213**|addi x4, x0, 7|x4 = **7**|
|44|**00410263**|beq x2, x4, offset|3 ≠ 7 → 不跳转|
|48|**06300293**|addi x5, x0, 99|x5 = **99**|
|4C|**000000EF**|jal x1, 0|x1 = PC+4（自跳）→ 程序停机|
|50|**02100313**|addi x6, x0, 33|不执行（已跳停）|
|54|**03700393**|addi x7, x0, 55|不执行|
|58|**00000EB7**|lui x29, 0xE|不执行|
```


| PC  | 指令 HEX       | 指令（汇编）             | 执行结果（寄存器与内存变化）            |
| --- | ------------ | ------------------ | ------------------------- |
| 00  | **00500093** | addi x1, x0, 5     | x1 = 5                    |
| 04  | **00300113** | addi x2, x0, 3     | x2 = 3                    |
| 08  | **002081B3** | add x3, x1, x2     | x3 = 5 + 3 = **8**        |
| 0C  | **40208233** | sub x4, x1, x2     | x4 = 5 - 3 = **2**        |
| 10  | **0030E2B3** | and x5, x1, x3     | 5 & 8 = 0 → x5 = **0**    |
| 14  | **0030F333** | or x6, x1, x3      | 5\|8 = 13 → x6 = **13**   |
| 18  | **00A0F3B3** | xor x7, x1, x10    | x10=0 → x7 = **5**        |
| 1C  | **0020F433** | slt x8, x1, x2     | 5<3 = false → x8 = **0**  |
| 20  | **00A0E4B3** | andi x9, x1, 10    | 5 & 10 = 0 → x9 = **0**   |
| 24  | **00A0E533** | ori x10, x1, 10    | 5\|10 = 15 → x10 = **15** |
| 28  | **00F0E5B3** | xori x11, x1, 15   | 5^15 = 10 → x11 = **10**  |
| 2C  | **00A0E633** | slti x12, x1, 10   | 5<10 → x12 = **1**        |
| 30  | **06400793** | addi x15, x0, 100  | x15 = 100                 |
| 34  | **00112023** | sw x1, 0(x2)       | MEM[x2=3] = **5**         |
| 38  | **00012183** | lw x3, 0(x2)       | x3 = MEM[3] = **5**       |
| 3C  | **00700193** | addi x3, x0, 7     | x3 = **7**                |
| 40  | **00700213** | addi x4, x0, 7     | x4 = **7**                |
| 44  | **00410263** | beq x2, x4, offset | 3 ≠ 7 → 不跳转               |
| 48  | **06300293** | addi x5, x0, 99    | x5 = **99**               |
| 4C  | **000000EF** | jal x1, 0          | x1 = PC+4（自跳）→ 程序停机       |
| 50  | **02100313** | addi x6, x0, 33    | 不执行（已跳停）                  |
| 54  | **03700393** | addi x7, x0, 55    | 不执行                       |
| 58  | **00000EB7** | lui x29, 0xE       | 不执行                       |





|     |      |            |                 |                   |
| --- | ---- | ---------- | --------------- | ----------------- |
|     | 0x0  | 0x00500093 | addi x1 x0 5    | addi x1, x0, 5    |
|     | 0x4  | 0x00300113 | addi x2 x0 3    | addi x2, x0, 3    |
|     | 0x8  | 0x002081B3 | add x3 x1 x2    | add x3, x1, x2    |
|     | 0xc  | 0x40208233 | sub x4 x1 x2    | sub x4, x1, x2    |
|     | 0x10 | 0x0030F2B3 | and x5 x1 x3    | and x5, x1, x3    |
|     | 0x14 | 0x0030E333 | or x6 x1 x3     | or x6, x1, x3     |
|     | 0x18 | 0x00A0C3B3 | xor x7 x1 x10   | xor x7, x1, x10   |
|     | 0x1c | 0x0020A433 | slt x8 x1 x2    | slt x8, x1, x2    |
|     | 0x20 | 0x00A0F493 | andi x9 x1 10   | andi x9, x1, 10   |
|     | 0x24 | 0x00A0E513 | ori x10 x1 10   | ori x10, x1, 10   |
|     | 0x28 | 0x00F0C593 | xori x11 x1 15  | xori x11, x1, 15  |
|     | 0x2c | 0x00A0A613 | slti x12 x1 10  | slti x12, x1, 10  |
|     | 0x30 | 0x06400793 | addi x15 x0 100 | addi x15, x0, 100 |
|     | 0x34 | 0x00112023 | sw x1 0(x2)     | sw x1, 0(x2)      |
|     | 0x38 | 0x00012183 | lw x3 0(x2)     | lw x3, 0(x2)      |
|     | 0x3c | 0x00700193 | addi x3 x0 7    | addi x3, x0, 7    |
|     | 0x40 | 0x00700213 | addi x4 x0 7    | addi x4, x0, 7    |
|     | 0x44 | 0x06300293 | addi x5 x0 99   | addi x5, x0, 99   |
|     | 0x48 | 0x02100313 | addi x6 x0 33   | addi x6, x0, 33   |
|     | 0x4c | 0x03700393 | addi x7 x0 55   | addi x7, x0, 55   |
|     | 0x50 | 0x0000EEB7 | lui x29 14      | lui x29, 0xE      |

|  序号 | 指令              | 操作          | 结果寄存器/存储 | 执行结果（HEX）      |
| --: | --------------- | ----------- | -------- | -------------- |
|   1 | add r3,r1,r2    | 5 + 3       | r3       | **0x00000008** |
|   2 | sub r4,r1,r2    | 5 - 3       | r4       | **0x00000002** |
|   3 | and r5,r1,r3    | 5 & 0xFFF0  | r5       | **0x00000000** |
|   4 | or r5,r1,r3     | 5 \| 0xFFF0 | r5       | **0xFFFFFFF5** |
|   5 | xor r6,r1,r3    | 5 ⊕ 0xFFF0  | r6       | **0xFFFFFFF5** |
|   6 | slt r6,r3,r1    | -16 < 5     | r6       | **0x00000001** |
|   7 | addi r4,r1,7    | 5 + 7       | r4       | **0x0000000C** |
|   8 | andi r5,r1,0x0F | 5 & 0x0F    | r5       | **0x00000005** |
|   9 | ori r5,r1,0xF0  | 5 \| 0xF0   | r5       | **0x000000F5** |
|  10 | xori r6,r1,0x0F | 5 ⊕ 0x0F    | r6       | **0x0000000A** |
|  11 | slti r6,r3,-8   | -16 < -8    | r6       | **0x00000001** |
|  12 | lw r4,0(r0)     | 从 MEM[0]    | r4       | **0x12345678** |
|  13 | sw r4,4(r0)     | 写入 MEM[4]   | MEM[4]   | **0x12345678** |
|  14 | beq r1,r2,L     | 不跳转         | PC       | **PC+4**       |
|  15 | jal L           | 跳转 + 保存返回址  | r31      | **PC+4**       |



| 地址   | 机器码        | 指令                | 解释             | 执行结果（HEX）             |     |
| ---- | ---------- | ----------------- | -------------- | --------------------- | --- |
| 0x00 | 0x00500093 | addi x1, x0, 5    | x1 = 5         | **0x00000005**        |     |
| 0x04 | 0x00300113 | addi x2, x0, 3    | x2 = 3         | **0x00000003**        |     |
| 0x08 | 0x002081B3 | add x3, x1, x2    | 5 + 3          | **0x00000008**        |     |
| 0x0C | 0x40208233 | sub x4, x1, x2    | 5 - 3          | **0x00000002**        |     |
| 0x10 | 0x0030F2B3 | and x5, x1, x3    | 5 & 8          | **0x00000000**        |     |
| 0x14 | 0x0030E333 | or x6, x1, x3     | 5 \| 8 = 13    | **0x0000000D**        |     |
| 0x18 | 0x00A0C3B3 | xor x7, x1, x10   | x7 = **5**     | **0x00000005**        |     |
| 0x1C | 0x0020A433 | slt x8, x1, x2    | 5 < 3 → 0      | **0x00000000**        |     |
| 0x20 | 0x00A0F493 | andi x9, x1, 10   | 5 & 10         | **0x00000000**        |     |
| 0x24 | 0x00A0E513 | ori x10, x1, 10   | 5 \| 10 = 15   | **0x0000000F**        |     |
| 0x28 | 0x00F0C593 | xori x11, x1, 15  | 5 ⊕ 15 = 10    | **0x0000000A**        |     |
| 0x2C | 0x00A0A613 | slti x12, x1, 10  | 5 < 10 → 1     | **0x00000001**        |     |
| 0x30 | 0x06400793 | addi x15, x0, 100 | x15 = 100      | **0x00000064**        |     |
| 0x34 | 0x00112023 | sw x1, 0(x2)      | MEM[x2+0] = x1 | 写入 x1=5               |     |
| 0x38 | 0x00012183 | lw x3, 0(x2)      | x3 = MEM[x2]   | 读取 5 → **0x00000005** |     |
| 0x3C | 0x00700193 | addi x3, x0, 7    | x3 = 7         | **0x00000007**        |     |
| 0x40 | 0x00700213 | addi x4, x0, 7    | x4 = 7         | **0x00000007**        |     |
| 0x44 | 0x06300293 | addi x5, x0, 99   | x5 = 99        | **0x00000063**        |     |
| 0x48 | 0x02100313 | addi x6, x0, 33   | x6 = 33        | **0x00000021**        |     |
| 0x4C | 0x03700393 | addi x7, x0, 55   | x7 = 55        | **0x00000037**        |     |
| 0x50 | 0x0000EEB7 | lui x29, 0xE      | x29 = 0xE0000  | **0x000E0000**        |     |
![[Pasted image 20251202210645.png]]





```
module SCPU_ctrl_more(
 input [4:0] OPcode, // Opcode------inst[6:2]
 input [2:0] Fun3, // Function-----inst[14:12]
 input Fun7, // Function-----inst[30]
 input MIO_ready, // CPU Wait
 output reg [2:0] ImmSel, // 立即数选择控制（位数扩宽）
 output reg ALUSrc_B, // 源操作数 2 选择
 output reg [1:0] MemtoReg, // 写回数据选择控制
 output reg [1:0] Jump, // jump（位数扩宽）
 output reg Branch, // beq
 output reg BranchN, // bne（新增）
 output reg RegWrite, // 寄存器写使能
 output reg MemRW, // 存储器读写使能
 output reg [3:0] ALU_Control, // alu 控制（位数扩宽）
 output reg CPU_MIO // not use
);

    reg [1:0] ALUop;
    wire [3:0] Fun = {Fun3, Fun7};

    /***********************
     * 主控制器
     ***********************/
    always @(*) begin
        // 默认值
        ALUSrc_B = 0;
        MemtoReg = 2'b00;
        RegWrite = 0;
        Branch   = 0;
        Jump     = 0;
        MemRW    = 0;
        CPU_MIO  = 0;
        ImmSel   = 2'b00;
        ALUop    = 2'b10; 

        case(OPcode)

            /*********************************
             * R-type : 01100
             *********************************/
            5'b01100: begin
                ALUSrc_B = 0;
                MemtoReg = 2'b00;
                RegWrite = 1;
                ALUop    = 2'b10;
            end

            /*********************************
             * Load : 00000  (lw)
             *********************************/
            5'b00000: begin
                ALUSrc_B = 1;
                MemtoReg = 2'b01;   // 写回 Memory 数据
                RegWrite = 1;
                MemRW    = 0;
                ImmSel   = 2'b00;   // I-type
                ALUop    = 2'b00;   // add 计算地址
            end

            /*********************************
             * Store : 01000 (sw)
             *********************************/
            5'b01000: begin
                ALUSrc_B = 1;
                RegWrite = 0;
                MemRW    = 1;
                ImmSel   = 2'b01;   // S-type
                ALUop    = 2'b00;   // add 地址
            end

            /*********************************
             * Branch : beq 11000
             *********************************/
            5'b11000: begin
                ALUSrc_B = 0;
                Branch   = 1;
                ImmSel   = 2'b10;   // B-type
                ALUop    = 2'b01;   // sub 比较
            end

            /*********************************
             * Jump : jal 11011
             *********************************/
            5'b11011: begin
                Jump     = 1;
                RegWrite = 1;
                MemtoReg = 2'b10;   // PC + 4
                ImmSel   = 2'b11;   // J-type
            end

            /*********************************
             * I-type 算术 : addi/xori/andi/ori... 00100
             *********************************/
            5'b00100: begin
                ALUSrc_B = 1;
                RegWrite = 1;
                MemtoReg = 2'b00;   // 写回 ALU 结果（修复严重错误）
                ImmSel   = 2'b00;   // I-type
                ALUop    = 2'b11;   // I-type ALU
            end
            
            5'b11000: begin // beq & bne
                ALUSrc_B = 0;
                MemtoReg = 0;
                RegWrite = 0;
                MemRW = 0;
                Jump = 0;
                ALUop = 2'b01;
                ImmSel = 3'b011;
                if (Fun3 == 3'b000) begin
                    Branch = 1;
                    BranchN = 0;
                end else begin
                    Branch = 0;
                    BranchN = 1;
                    end
                end
                
                
            5'b11001: begin // jalr
                ALUSrc_B = 1;
                MemtoReg = 2'b10;
                RegWrite = 1;
                MemRW = 0;
                Branch = 0;
                BranchN = 0;
                Jump = 2;
                ALUop = 2'b00;
                ImmSel = 3'b001;
                end
                
                
            5'b01101: begin // lui
                ALUSrc_B = 0;
                MemtoReg = 2'b11;
                RegWrite = 1;
                MemRW = 0;
                Branch = 0;
                BranchN = 0;
                Jump = 0;
                ALUop = 2'b00;
                ImmSel = 3'b000;
            end
            
            default: begin
                // 全保持默认值
            end
        endcase
    end

    /***********************
     * ALU 控制器
     * 对应 ALU_wrapper 中的 mux8to1_32
     *
     * sel 对照:
     * 000 and
     * 001 or
     * 010 add
     * 011 xor
     * 100 nor
     * 101 srl
     * 110 sub
     * 111 slt
     ***********************/
    always @(*) begin
        case(ALUop)

            // Load/Store: add
            2'b00: ALU_Control = 3'b010;

            // Branch: sub
            2'b01: ALU_Control = 3'b110;

            // R-type
            2'b10: begin
                case(Fun)
                    4'b0000: ALU_Control = 3'b010; // add
                    4'b0001: ALU_Control = 3'b110; // sub
                    4'b1110: ALU_Control = 3'b000; // and
                    4'b1100: ALU_Control = 3'b001; // or
                    4'b1000: ALU_Control = 3'b011; // xor
                    4'b0100: ALU_Control = 3'b111; // slt
                    4'b1010: ALU_Control = 3'b101; // srl
                    default: ALU_Control = 3'b000;
                endcase
            end

            // I-type 算术（与 R-type 相同，但只依赖 Fun3）
            2'b11: begin
                case(Fun3)
                    3'b000: ALU_Control = 4'b0010; // addi
                     3'b010: ALU_Control = 4'b0111; // slti
                     3'b011: ALU_Control = 4'b1001; // sltiu
                     3'b100: ALU_Control = 4'b1100; // xori
                     3'b110: ALU_Control = 4'b0001; // ori
                     3'b111: ALU_Control = 4'b0000; // andi
                     3'b001: ALU_Control = 4'b1110; // slli
                     3'b101:
                        if (Fun7 == 0)
                            ALU_Control = 4'b1101; // srli
                        else
                            ALU_Control = 4'b1111; // srai
                     default: ALU_Control = 4'bx;
                endcase
            end

            default: ALU_Control = 3'b000;
        endcase
    end
endmodule


```





```


module SCPU_ctrl( 
input[4:0]OPcode, //Opcode------inst[6:2]
input[2:0]Fun3, //Function-----inst[14:12]
input Fun7, //Function-----inst[30]
input MIO_ready, //CPU Wait

output reg [1:0]ImmSel, //立即数选择控制
output reg ALUSrc_B, //源操作数2选择
output reg [1:0]MemtoReg, //写回数据选择控制
output reg Jump, //jal
output reg Branch, //beq
output reg RegWrite, //寄存器写使能
output reg MemRW, //存储器读写使能
output reg [2:0]ALU_Control, //alu控制
output reg CPU_MIO //not use
);
  
    reg [1:0] ALUop;
    wire[3:0] Fun;

  always @(*) begin
        ALUSrc_B=0;
        MemtoReg=0;
        RegWrite=0;
        Branch=0;
        Jump=0;
        MemRW=0;
        CPU_MIO=0;
        ALUop=2'b10;
        case(OPcode)
            5'b01100: begin 
                ALUSrc_B = 0;
                MemtoReg = 2'b00;
                RegWrite = 1;
                MemRW = 0;
                Branch = 0;
                Jump = 0;
                ALUop = 2'b10;
                ImmSel = 2'b00;
            end //ALU
            5'b00000: begin 
                ALUSrc_B = 1;
                MemtoReg = 2'b01;
                RegWrite = 1;
                MemRW = 0;
                Branch = 0;
                Jump = 0;
                ALUop = 2'b00;
                ImmSel = 2'b00;    
             end //load
            5'b01000: begin 
                ALUSrc_B = 1;
                //MemtoReg = 0;
                RegWrite = 0;
                MemRW = 1;
                Branch = 0;
                Jump = 0;
                ALUop = 2'b00;
                ImmSel = 2'b01; 
            end //store
            5'b11000: begin 
                ALUSrc_B = 0;
                //MemtoReg = 0;
                RegWrite = 0;
                MemRW = 0;
                Branch = 1;
                Jump = 0;
                ALUop = 2'b01;
                ImmSel = 2'b10;
                     end //beq
            5'b11011: begin 
                ALUSrc_B = 1;
                MemtoReg = 2'b10;
                 RegWrite = 1;
                MemRW = 0;
                Branch = 0;
                Jump = 1;
                //ALUop = 2'b01;
                ImmSel = 2'b11;
                     end //jump
            5'b00100: begin
                ALUSrc_B = 1;
                MemtoReg = 2'b10;
                RegWrite = 1;
                MemRW = 0;
                Branch = 0;
                Jump = 0;
                //ALUop = 2'b00;
                ImmSel = 2'b00; 
                     end //ALU(addi, …)
            default: begin
                ALUSrc_B = 0;
                MemtoReg = 2'b00;
                RegWrite = 0;
                MemRW = 0;
                Branch = 0;
                Jump = 0;
                ALUop = 2'b00;
                ImmSel = 2'b00; 
                     end 
            endcase
        end
        
        
      assign Fun = {Fun3,Fun7};
      always @* begin
            case(ALUop)
                2'b00: ALU_Control =3'b010; //add计算地址
                2'b01: ALU_Control = 3'b110 ; //sub比较条件
                2'b10: 
                    case(Fun)
                        4'b0000: ALU_Control = 3'b010 ; //add
                        4'b0001: ALU_Control = 3'b110 ; //sub
                        4'b1110: ALU_Control = 3'b000; //and
                        4'b1100: ALU_Control = 3'b001 ; //or
                        4'b0100: ALU_Control = 3'b111 ; //slt
                        4'b1010: ALU_Control = 3'b101 ; //srl
                        4'b1000: ALU_Control = 3'b011 ; //xor    
                        default: ALU_Control=3'bx;
                    endcase
                2'b11: 
                    case(Fun3)
                        3'b000:ALU_Control=3'b010;
                        3'b111:ALU_Control=3'b000;
                        3'b110:ALU_Control=3'b001;
                        3'b010:ALU_Control=3'b111;
                        3'b101:ALU_Control=3'b101;
                        3'b100:ALU_Control=3'b011;
                        default: ALU_Control=3'bx;
                    endcase        
            endcase
       end
endmodule


```


```

module SCPU_ctrl(
    input [4:0] OPcode, // inst[6:2]
    input [2:0] Fun3,   // inst[14:12]
    input       Fun7,   // inst[30]
    input       MIO_ready,

    output reg [1:0] ImmSel,    // 00: I, 01: S, 10: B, 11: J/U (you can adapt)
    output reg       ALUSrc_B,
    output reg [1:0] MemtoReg,  // 00: ALU, 01: MEM, 10: PC+4
    output reg       Jump,
    output reg       Branch,
    output reg       RegWrite,
    output reg       MemRW,
    output reg [2:0] ALU_Control,
    output reg       CPU_MIO
);

    reg [1:0] ALUop;
    wire [3:0] Fun;
    assign Fun = {Fun3, Fun7}; // {fun3[2:0], fun7[0]} -> 4'b0000 for add (fun3=000,fun7=0)

    // default values to avoid inferred latches
    always @(*) begin
        // default (safe) assignments
        ALUSrc_B = 0;
        MemtoReg = 2'b00;  // default: ALU result
        RegWrite = 0;
        Branch   = 0;
        Jump     = 0;
        MemRW    = 0;
        CPU_MIO  = 0;
        ALUop    = 2'b00;  // default: add (for load/store)
        ImmSel   = 2'b00;
        
        case (OPcode)
            // R-type (ALU register)
            5'b01100: begin
                ALUSrc_B = 0;
                MemtoReg = 2'b00;
                RegWrite = 1;
                MemRW    = 0;
                Branch   = 0;
                Jump     = 0;
                ALUop    = 2'b10; // R-type
                ImmSel   = 2'b00;
            end

            // load (I-type addressing)
            5'b00000: begin
                ALUSrc_B = 1;
                MemtoReg = 2'b01; // write back from MEM
                RegWrite = 1;
                MemRW    = 0;
                Branch   = 0;
                Jump     = 0;
                ALUop    = 2'b00; // add for address calc
                ImmSel   = 2'b00; // I-type immediate
            end

            // store (S-type)
            5'b01000: begin
                ALUSrc_B = 1;
                MemtoReg = 2'b00; // N/A
                RegWrite = 0;
                MemRW    = 1;     // write memory
                Branch   = 0;
                Jump     = 0;
                ALUop    = 2'b00; // add for address calc
                ImmSel   = 2'b01; // S-type immediate
            end

            // branch (B-type)
            5'b11000: begin
                ALUSrc_B = 0;
                MemtoReg = 2'b00;
                RegWrite = 0;
                MemRW    = 0;
                Branch   = 1;
                Jump     = 0;
                ALUop    = 2'b01; // sub for comparison
                ImmSel   = 2'b10; // B-type immediate
            end

            // jal (J-type / link)
            5'b11011: begin
                ALUSrc_B = 0;
                MemtoReg = 2'b10; // PC+4 -> write-back
                RegWrite = 1;     // write link register
                MemRW    = 0;
                Branch   = 0;
                Jump     = 1;
                ALUop    = 2'b00; // ALU not used; keep safe default
                ImmSel   = 2'b11; // J-type immediate
            end

            // ALU immediate (addi, andi, ori, xori, slti)
            5'b00100: begin
                ALUSrc_B = 1;
                MemtoReg = 2'b00; // ALU result
                RegWrite = 1;
                MemRW    = 0;
                Branch   = 0;
                Jump     = 0;
                ALUop    = 2'b11; // I-type ALU ops (use Fun3)
                ImmSel   = 2'b00; // I-type immediate
            end

            default: begin
                // keep defaults (safe)
                ALUSrc_B = 0;
                MemtoReg = 2'b00;
                RegWrite = 0;
                MemRW    = 0;
                Branch   = 0;
                Jump     = 0;
                ALUop    = 2'b00;
                ImmSel   = 2'b00;
            end
        endcase
    end

    // ALU control unit: ALUop mapping
    // ALU_Control encodings must match your ALU module!
    // Here we choose:
    // 3'b010 = ADD
    // 3'b110 = SUB
    // 3'b000 = AND
    // 3'b001 = OR
    // 3'b111 = SLT
    // 3'b101 = SRL (right shift logical)
    // 3'b011 = XOR
    always @(*) begin
        case (ALUop)
            2'b00: ALU_Control = 3'b010; // load/store address calc -> add
            2'b01: ALU_Control = 3'b110; // branch -> sub (for compare)
            2'b10: begin                 // R-type: use Fun = {fun3,fun7}
                case (Fun)
                    4'b0000: ALU_Control = 3'b010; // add (fun3=000,fun7=0)
                    4'b0001: ALU_Control = 3'b110; // sub (fun3=000,fun7=1)
                    4'b1110: ALU_Control = 3'b000; // and (fun3=111,fun7=0) adjust if necessary
                    4'b1100: ALU_Control = 3'b001; // or  (fun3=110)
                    4'b0100: ALU_Control = 3'b111; // slt (fun3=010)
                    4'b1010: ALU_Control = 3'b101; // srl (fun3=101,fun7=0) 
                    4'b1000: ALU_Control = 3'b011; // xor (fun3=100)
                    default: ALU_Control = 3'b010;
                endcase
            end
            2'b11: begin // I-type ALU ops -> decode by fun3
                case (Fun3)
                    3'b000: ALU_Control = 3'b010; // addi
                    3'b111: ALU_Control = 3'b000; // andi
                    3'b110: ALU_Control = 3'b001; // ori
                    3'b010: ALU_Control = 3'b111; // slti
                    3'b101: ALU_Control = 3'b101; // srli (if supported)
                    3'b100: ALU_Control = 3'b011; // xori
                    default: ALU_Control = 3'b010;
                endcase
            end
            default: ALU_Control = 3'b010;
        endcase
    end
endmodule


```

```

module SCPU_ctrl_more( 
    input  [4:0] OPcode,      // inst[6:2]
    input  [2:0] Fun3,        // inst[14:12]
    input        Fun7,        // inst[30]
    input        MIO_ready,

    output reg [1:0] ImmSel, 
    output reg       ALUSrc_B,
    output reg [1:0] MemtoReg,
    output reg       Jump,
    output reg       Branch,
    output reg       RegWrite,
    output reg       MemRW,
    output reg [2:0] ALU_Control,
    output reg       CPU_MIO
);


    reg [1:0] ALUop;
    wire [3:0] Fun = {Fun3, Fun7}; // [3:0] used for R-type decode

    always @(*) begin
        // default values
        ALUSrc_B   = 1'b0;
        MemtoReg   = 2'b00;
        RegWrite   = 1'b0;
        Branch     = 1'b0;
        BranchN    = 1'b0;
        Jump       = 2'b00;
        MemRW      = 1'b0;
        CPU_MIO    = 1'b0;
        ImmSel     = 3'b000;
        ALUop      = 2'b10; // default assume R-type
        ALU_Control= 4'b0000;

        case (OPcode)
            // R-type (01100)
            5'b01100: begin
                ALUSrc_B = 1'b0;
                MemtoReg = 2'b00;
                RegWrite = 1'b1;
                ALUop    = 2'b10;
                ImmSel   = 3'b000;
            end

            // Load (lw) 00000
            5'b00000: begin
                ALUSrc_B = 1'b1;
                MemtoReg = 2'b01; // write back from memory
                RegWrite = 1'b1;
                MemRW    = 1'b0;
                ImmSel   = 3'b000; // I-type
                ALUop    = 2'b00;  // add for address
            end

            // Store (sw) 01000
            5'b01000: begin
                ALUSrc_B = 1'b1;
                RegWrite = 1'b0;
                MemRW    = 1'b1;
                ImmSel   = 3'b001; // S-type
                ALUop    = 2'b00;  // add for address
            end

            // Branch (beq / bne) 11000
            5'b11000: begin
                ALUSrc_B = 1'b0;
                RegWrite = 1'b0;
                MemRW    = 1'b0;
                Jump     = 2'b00;
                ImmSel   = 3'b010; // B-type
                ALUop    = 2'b01;  // sub for comparison
                // Fun3 selects beq/bne (RV32I: funct3==000 -> beq, 001 -> bne)
                if (Fun3 == 3'b000) begin
                    Branch  = 1'b1;
                    BranchN = 1'b0;
                end else if (Fun3 == 3'b001) begin
                    Branch  = 1'b0;
                    BranchN = 1'b1;
                end else begin
                    Branch  = 1'b0;
                    BranchN = 1'b0;
                end
            end

            // JAL 11011
            5'b11011: begin
                Jump     = 2'b01;
                RegWrite = 1'b1;
                MemtoReg = 2'b10;   // write PC+4
                ImmSel   = 3'b011;  // J-type
                ALUop    = 2'b00;   // used default
            end

            // JALR 11001
            5'b11001: begin
                Jump     = 2'b10;
                ALUSrc_B = 1'b1;    // for computing target (rs1 + imm)
                RegWrite = 1'b1;
                MemtoReg = 2'b10;   // write PC+4
                MemRW    = 1'b0;
                Branch   = 1'b0;
                BranchN  = 1'b0;
                ImmSel   = 3'b000;  // I-type immediate for jalr
                ALUop    = 2'b00;   // add to compute target
            end

            // I-type arithmetic (addi, xori, ...): 00100
            5'b00100: begin
                ALUSrc_B = 1'b1;
                RegWrite = 1'b1;
                MemtoReg = 2'b00;   // ALU result
                ImmSel   = 3'b000;  // I-type
                ALUop    = 2'b11;   // I-type ALU decode
            end

            // LUI 01101
            5'b01101: begin
                ALUSrc_B = 1'b0;
                MemtoReg = 2'b11;   // ImmGen output (LUI)
                RegWrite = 1'b1;
                MemRW    = 1'b0;
                Jump     = 2'b00;
                ImmSel   = 3'b100;  // treat as LUI selector (user-defined)
                ALUop    = 2'b00;
            end

            default: begin
                // keep defaults
            end
        endcase
    end

    // ALU control: produce 4-bit ALU_Control based on ALUop and fun fields
    always @(*) begin
        case (ALUop)
            2'b00: begin
                // load/store: add
                ALU_Control = 4'b0010; // add
            end
            2'b01: begin
                // branch: sub
                ALU_Control = 4'b0110; // sub
            end
            2'b10: begin
                // R-type: use Fun (Fun3, Fun7)
                case (Fun)
                    4'b0000: ALU_Control = 4'b0010; // add
                    4'b0001: ALU_Control = 4'b0110; // sub
                    4'b1110: ALU_Control = 4'b0000; // and
                    4'b1100: ALU_Control = 4'b0001; // or
                    4'b1000: ALU_Control = 4'b0011; // xor
                    4'b0100: ALU_Control = 4'b0111; // slt
                    4'b1010: ALU_Control = 4'b1010; // srl (logical)
                    default: ALU_Control = 4'b0000;
                endcase
            end
            2'b11: begin
                // I-type arithmetic, decode by Fun3 (and Fun7 for shifts)
                case (Fun3)
                    3'b000: ALU_Control = 4'b0010; // addi
                    3'b010: ALU_Control = 4'b0111; // slti
                    3'b011: ALU_Control = 4'b1001; // sltiu (user enc)
                    3'b100: ALU_Control = 4'b0011; // xori
                    3'b110: ALU_Control = 4'b0001; // ori
                    3'b111: ALU_Control = 4'b0000; // andi
                    3'b001: ALU_Control = 4'b1110; // slli (user enc)
                    3'b101: begin
                        if (Fun7 == 1'b0)
                            ALU_Control = 4'b1011; // srli (user enc)
                        else
                            ALU_Control = 4'b1111; // srai (user enc)
                    end
                    default: ALU_Control = 4'b0000;
                endcase
            end
            default: ALU_Control = 4'b0000;
        endcase
    end

endmodule

```



```

```