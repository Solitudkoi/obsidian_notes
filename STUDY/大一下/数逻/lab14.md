![[Pasted image 20250530100120.png]]
![[Pasted image 20250530100742.png]]
set_property PACKAGE_PIN AA13 [get_ports {sw[2]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[2]}]
set_property PACKAGE_PIN AA12 [get_ports {sw[3]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[3]}]
set_property PACKAGE_PIN Y13 [get_ports {sw[4]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[4]}]
set_property PACKAGE_PIN Y12 [get_ports {sw[5]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[5]}]
set_property PACKAGE_PIN AD11 [get_ports {sw[6]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[6]}]
set_property PACKAGE_PIN AD10 [get_ports {sw[7]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[7]}]
set_property PACKAGE_PIN AE10 [get_ports {sw[8]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[8]}]
set_property PACKAGE_PIN AE12 [get_ports {sw[9]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[9]}]
set_property PACKAGE_PIN AF12 [get_ports {sw[10]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[10]}]
set_property PACKAGE_PIN AE8 [get_ports {sw[11]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[11]}]
set_property PACKAGE_PIN AF8 [get_ports {sw[12]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[12]}]
set_property PACKAGE_PIN AE13 [get_ports {sw[13]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[13]}]
set_property PACKAGE_PIN AF13 [get_ports {sw[14]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[14]}]
set_property PACKAGE_PIN AF10 [get_ports {sw[15]}]
set_property IOSTANDARD LVCMOS15 [get_ports {sw[15]}]





set_property PACKAGE_PIN DISP4 [get_ports seg_en[4]]
set_property PACKAGE_PIN DISP5 [get_ports seg_en[5]]







`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/05/30 10:27:14
// Design Name: 
// Module Name: top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module top(
    input clk,
    input wire [15:0]sw,
    output SEGDT,
    output SEGCLK,
    output SEGCLR,
    output SEGEN
    );
    wire clk100,CR1,s1;
    wire [6:0] Co;
    wire [63:0] num;
    wire [3:0] a,b,c,d,e,f,g;
    clk_100 clk_100_inst(
        .clk(clk),
        .clk_100(clk100)
        );
    mands_one mands_one_inst(
        .clk(clk100),
        .CR(sw[0]),
        .CTP(1'b1),
        .CTT(1'b1),
        .L_D(sw[1]),
        .D(4'b0000),
        .Q(a),
        .CO(Co[0])
        );
    mands_one mands_one_inst1(
        .clk(clk100),
        .CR(sw[0]),
        .CTP(1'b1),
        .CTT(Co[0]),
        .L_D(sw[1]),
        .D(4'b0000),
        .Q(b),
        .CO(Co[1])
        );
    mands_ten mands_ten_inst(
        .clk(clk100),
        .CR(sw[0]),
        .CTP(1'b1),
        .CTT(Co[1]),
        .L_D(sw[1]),
        .D(4'b0000),
        .Q(c),
        .CO(Co[2])
        );
    mands_one mands_one_inst23232(
        .clk(clk100),
        .CR(sw[0]),
        .CTP(1'b1),
        .CTT(Co[2]),
        .L_D(sw[1]),
        .D(4'b0000),
        .Q(d),
        .CO(Co[3])
        );
    mands_ten mands_ten_inst1(
        .clk(clk100),
        .CR(sw[0]),
        .CTP(1'b1),
        .CTT(Co[3]),
        .L_D(sw[1]),
        .D(4'b0000),
        .Q(e),
        .CO(Co[4])
        );
    hour_one hour_one_inst(
        .clk(clk100),
        .CR(s1),
        .CTP(1'b1),
        .CTT(Co[4]),
        .L_D(sw[1]),
        .D(4'b0011),
        .Q(f),
        .CO(Co[5])
        );
    hour3 hour3_inst(
        .clk(clk100),
        .CR(s1),
        .CTP(1'b1),
        .CTT(Co[5]),
        .L_D(sw[1]),
        .D(4'b0010),
        .Q(g),
        .CO(Co[6])
        );
    assign s1=sw[0]&~(~g[3]&~g[2]&~g[1]&g[0]&f[3]&~f[2]&~f[1]&~f[0]);
    wire [3:0]A;
    assign A=0;
    MyMC14495 MyMC14495_inst0 (
        .D0(A[0]),
        .D1(A[1]),
        .D2(A[2]),
        .D3(A[3]),
        .LE(1'b0),
        .point(1'b0),
        .a(num[7]),
        .b(num[6]),
        .c(num[5]),
        .d(num[4]),
        .e(num[3]),
        .f(num[2]),
        .g(num[1]),
        .p(num[0])
        );
    MyMC14495 MyMC14495_inst1 (
        .D0(g[0]),
        .D1(g[1]),
        .D2(g[2]),
        .D3(g[3]),
        .LE(1'b0),
        .point(1'b0),
        .a(num[15]),
        .b(num[14]),
        .c(num[13]),
        .d(num[12]),
        .e(num[11]),
        .f(num[10]),
        .g(num[9]),
        .p(num[8])
        );
    MyMC14495 MyMC14495_inst2 (
        .D0(f[0]),
        .D1(f[1]),
        .D2(f[2]),
        .D3(f[3]),
        .LE(1'b0),
        .point(1'b0),
        .a(num[23]),
        .b(num[22]),
        .c(num[21]),
        .d(num[20]),
        .e(num[19]),
        .f(num[18]),
        .g(num[17]),
        .p(num[16])
        );
    MyMC14495 MyMC14495_inst3 (
        .D0(e[0]),
        .D1(e[1]),
        .D2(e[2]),
        .D3(e[3]),
        .LE(1'b0),
        .point(1'b0),
        .a(num[31]),
        .b(num[30]),
        .c(num[29]),
        .d(num[28]),
        .e(num[27]),
        .f(num[26]),
        .g(num[25]),
        .p(num[24])
        );
    MyMC14495 MyMC14495_inst4 (
        .D0(d[0]),
        .D1(d[1]),
        .D2(d[2]),
        .D3(d[3]),
        .LE(1'b0),
        .point(1'b0),
        .a(num[39]),
        .b(num[38]),
        .c(num[37]),
        .d(num[36]),
        .e(num[35]),
        .f(num[34]),
        .g(num[33]),
        .p(num[32])
        );
    MyMC14495 MyMC14495_inst5 (
        .D0(c[0]),
        .D1(c[1]),
        .D2(c[2]),
        .D3(c[3]),
        .LE(1'b0),
        .point(1'b0),
        .a(num[47]),
        .b(num[46]),
        .c(num[45]),
        .d(num[44]),
        .e(num[43]),
        .f(num[42]),
        .g(num[41]),
        .p(num[40])
        );
    MyMC14495 MyMC14495_inst6 (
        .D0(b[0]),
        .D1(b[1]),
        .D2(b[2]),
        .D3(b[3]),
        .LE(1'b0),
        .point(1'b0),
        .a(num[55]),
        .b(num[54]),
        .c(num[53]),
        .d(num[52]),
        .e(num[51]),
        .f(num[50]),
        .g(num[49]),
        .p(num[48])
        );
    MyMC14495 MyMC14495_inst7 (
        .D0(a[0]),
        .D1(a[1]),
        .D2(a[2]),
        .D3(a[3]),
        .LE(1'b0),
        .point(1'b0),
        .a(num[63]),
        .b(num[62]),
        .c(num[61]),
        .d(num[60]),
        .e(num[59]),
        .f(num[58]),
        .g(num[57]),
        .p(num[56])
        ); 
        wire [31:0] div;
        wire finish;
        clkdiv clkdiv_inst(
            .clk(clk),
            .rst(1'b0),
            .div_res(div)
            );
    SEGDRV SEGDRV_inst(
        .load(div[7]),
        .clk(clk),
        .in(num),
        .dout(SEGDT),
        .finish(finish)
        );
    assign SEGCLK=clk|finish;
    assign SEGCLR=1'b1;
    assign SEGEN=1'b1;
endmodule
















SEGDRV SEGDRV_inst(
        .load(div[7]),
        .clk(clk),
        .in(num),
        .dout(SEGDT),
        .finish(finish)
        );

|                             |
| --------------------------- |
| module LED_DRV(             |
| input clk,                  |
| input load,                 |
| input [15:0] in,            |
| output reg LED_D0,          |
| output reg finish           |
| );                          |
| reg [4:0] cnt;              |
| reg [15:0] data=0;          |
| always @(posedge clk) begin |
| if (load) begin             |
| data<=in;                   |
| cnt<=0;                     |
| end                         |
| else begin                  |
| LED_D0<=data[0];            |
| data<={1'b1,data[15:1]};    |
| cnt<=cnt+1'b1;              |
| if(cnt>5'b10000)begin       |
| finish<=1'b0;               |
| cnt<=5'b10000;              |
| end                         |
| else begin                  |
| finish<=1'b1;               |
| end                         |
| end                         |
| end                         |
| endmodule                   |

















module ShiftReg32b(
    input clk,
    input load,
    input [63:0] in,
    output reg dout,
    output reg finish
    );
    reg [63:0] mid=0;
    reg [6:0] cnt=0;
    always@(posedge clk) begin
        if(load) begin
            mid<=in;
            cnt<=0;
        end
        else begin
            mid <={1'b1,mid[63:1]};
            dout<=mid[0];
            cnt<=cnt+1'b1;
            if(cnt>=7'b1000000) begin
                finish<=1'b1;
                cnt<=7'b1000000;
            end
            else begin
                finish<=1'b0;
            end
        end
    end
endmodule