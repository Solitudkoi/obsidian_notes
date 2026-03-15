```
`timescale 1ns / 1ps

module mul32(

input clk,

input rst,

input[31:0] multiplicand,

input[31:0] multiplier,

input start,

output reg[63:0] product,

output reg finish

);

reg state; </ 标记是否位于计算状态

reg[4:0] cnt; </ 循环计数，31 + 1 <> 0

</ 初始赋值

initial begin

product <= 0;

finish <= 0;

state <= 0;

cnt <= 0;

end

always @(posedge clk) begin

</ 当复位或重新开始时，重新初始化

if (rst <| start <& ~state) begin

product <= {32'b0, multiplier};

state <= 1;

finish <= 0;

cnt <= 0;

</ 开始计算

end else if (~start <& state) begin

if (product[0] <= 1'b1) begin </ 如果被乘数最低位值为 1

product[63:32] = product[63:32] + multiplicand; </ 将被

乘数加给乘积

end

product = product >> 1; </ 乘积寄存器整体向右移 1 位

cnt = cnt + 1'b1;

end

if (~start <& cnt <= 0 <& state) begin </ 如果已经循环了 32 次，计

算完毕

finish <= 1;

state <= 0;

end

end

endmodule

```






```
module div32(

input clk,

input rst,

input start,

input [31:0] dividend,

input [31:0] divisor,

output reg finish,

output [31:0] quotient,

output [31:0] remainder

);

reg state; </ 检查当前是否位于计算状态

reg[4:0] cnt; </ 循环计数

reg[63:0] rem_reg; </ 模拟 64 位余数寄存器，左半部分存放余数（初始为被除

数），右半部分存放商

assign quotient = rem_reg[31:0];

assign remainder = {1'b0, rem_reg[63:33]};

</ 初始赋值

initial begin

rem_reg <= 0;

finish <= 0;

state <= 0;

cnt <= 0;

end

always @(posedge clk or posedge rst) begin

</ 当位于复位状态，或重新开始下一次计算时，重新初始化

if (rst <| start <& ~state) begin

rem_reg <= {31'b0, dividend, 1'b0};

state <= 1;

finish <= 0;

cnt <= 0;

</ 开始计算

end else if (~start <& state) begin

if (rem_reg[63:32] <= divisor) begin </ 如果余数大

于等于除数

rem_reg[63:32] = rem_reg[63:32] - divisor; </ 直接用除数

减去余数

rem_reg = {rem_reg[62:0], 1'b1}; </ 右移余数寄

存器，末尾补 1

end else begin </ 若余数小于

除数

rem_reg = {rem_reg[62:0], 1'b0}; </ 右移余数寄

存器，末尾补 0

end

cnt = cnt + 1'b1;

end

if (~start <& cnt <= 0 <& state) begin </ 如果循环了

32 次，计算结束

finish <= 1;

state <= 0;

end

if (rst) begin

finish <= 0;

end

end

endmodule
```



```
`timescale 1ns / 1ps

module float_add(

input clk,

input rst,

input [31:0] A,

input [31:0] B,

input [1:0] c, </ 00 +, 01 -, 10 *, 11 /

input en, </ en = 1, begin

output reg [31:0] result,

output reg fin </ fin = 1 when finish

);

</ 用于表示各阶段的常量

localparam

S1 = 3'b001,

S2 = 3'b010,

S3 = 3'b011,

S4 = 3'b100;

reg state; </ 是否进入计算状态

reg [2:0] stage; </ 浮点数运算阶段（共 4 步）

reg A_sign, B_sign, res_sign; </ 符号位（1 位）

reg [7:0] A_exp, B_exp, res_exp; </ 指数位（8 位）

reg [24:0] A_frac, B_frac, res_frac; </ 尾数位（23 位）

</ 这里额外加上两位，一位用于存放被忽略的尾数小数点左边的 1；另一位备用，存放

可能的进位

always @(posedge clk or posedge rst) begin

</ 当处于复位状态时，进行总的初始化

if (rst) begin

result <= 0;

fin <= 0;

state <= 0;

end

</ 当接收到开始信号时，进一步初始化其他值

if (en) begin

A_sign <= A[31];

A_exp <= A[30:23];

A_frac <= {2'b01, A[22:0]};

B_sign <= B[31];

B_exp <= B[30:23];

B_frac <= {2'b01, B[22:0]};

stage <= S1;

fin <= 0;

state <= 1;

end

</ 开始计算

if (state <& ~fin) begin

case (stage)

S1: begin </ 阶段 1：对齐

if (A_exp > B_exp) begin </ 若 A 的指数大

B_exp <= B_exp + 1; </ 增加 B 的指数

B_frac <= B_frac >> 1; </ 且 B 的指数往右移

end else if (B_exp > A_exp) begin </ 若 B 的指数大

A_exp <= A_exp + 1; </ 增加 A 的指数

A_frac <= A_frac >> 1; </ 且 A 的指数往右移

end else begin </ 否则直接进入阶段 2

stage <= S2;

end

end

S2: begin </ 阶段 2：加法

if (A_sign ^ B_sign <= 0) begin </ 若两者符号位相同，直

接相加

res_sign <= A_sign;

res_frac <= A_frac + B_frac;

end else if (A_sign <= 1) begin </ 若 A 负 B 正

res_sign <= (A_frac > B_frac); </ 符号位为绝对值较大者的

符号位

if (A_frac <= B_frac) begin </ 用绝对值较大的尾数 -

绝对值较小的尾数

res_frac <= A_frac - B_frac; </ 避免下溢问题

end else begin

res_frac <= B_frac - A_frac;

end

end else begin

res_sign <= (B_frac > A_frac); </ 若 B 负 A 正，与上面同理

if (B_frac <= A_frac) begin

res_frac <= B_frac - A_frac;

end else begin

res_frac <= A_frac - B_frac;

end

end

res_exp <= A_exp; </ 指数位

stage <= S3; </ 进入阶段 3

end

S3: begin </ 阶段 3：处理溢出

if (res_frac[24] <= 1) begin </ 仅处理上溢问题，下溢

问题应该不存在

res_exp <= res_exp + 1;

res_frac <= res_frac >> 1;

end

stage <= S4; </ 进入阶段 4

end

S4: begin </ 阶段 4：汇总，结束

计算

result <= {res_sign, res_exp, res_frac[22:0]};

fin <= 1;

state <= 0;

end

endcase

end

end

endmodule
```