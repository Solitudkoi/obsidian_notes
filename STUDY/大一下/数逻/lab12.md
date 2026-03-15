`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/05/22 16:43:57
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
    input [1:0] SW,
    input SW1,
    input clk,
    output [7:0] SEGMENT,
    output [3:0] AN,
    output BTN_X
    );
    assign BTN_X = 1'b0;
    wire Load_A, Co;
    wire [3:0] A,  A_IN, A1;
    wire [31:0] clk_div;
    MyLoad p0(
        .clk(clk), 
        .clk_1ms(clk_div[17]),
        .btn_in(SW[1]),
        .Load_out(Load_A)
    );
    MyRegister4b p1(
        .load(Load_A),
        .in(A_IN),
        .out(A)
    );
    clkdiv p2(
        .clk(clk),
        .rst(1'b0),
        .div_res(clk_div)
    );
    assign A_IN = (SW1 == 1'b0)? A1: 4'b0000; 
    AddSub4b p3(
        .A(A),
        .B(4'b0001),
        .Ctrl(SW[0]),
        .S(A1),
        .Cout(Co)
    );
    Displaynumber p4(
        .clk(clk),
        .rst(1'b0),
        .hexs({A,A1,A_IN,4'b0000}),
        .points(4'b0000),
        .LEs(4'b0),
        .AN(AN),
        .SEGMENT(SEGMENT)
    );
endmodule






set_property PACKAGE_PIN AC18 [get_ports clk]
set_property IOSTANDARD LVCMOS18 [get_ports clk]

create_clock -period 10.000 -name clk [get_ports "clk"]
set_property PACKAGE_PIN V14 [get_ports {SW[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {SW[1]}]
set_property PACKAGE_PIN AB10 [get_ports {SW[0]}]
set_property IOSTANDARD LVCMOS15 [get_ports {SW[0]}]
set_property PACKAGE_PIN AA10 [get_ports {SW1}]
set_property IOSTANDARD LVCMOS15 [get_ports {SW1}]
set_property PACKAGE_PIN W16 [get_ports BTN_X]
set_property IOSTANDARD LVCMOS18 [get_ports BTN_X]
set_property PACKAGE_PIN AD21 [get_ports {AN[0]}]
set_property PACKAGE_PIN AC21 [get_ports {AN[1]}]
set_property PACKAGE_PIN AB21 [get_ports {AN[2]}]
set_property PACKAGE_PIN AC22 [get_ports {AN[3]}]
set_property PACKAGE_PIN AB22 [get_ports {SEGMENT[0]}]
set_property PACKAGE_PIN AD24 [get_ports {SEGMENT[1]}]
set_property PACKAGE_PIN AD23 [get_ports {SEGMENT[2]}]
set_property PACKAGE_PIN Y21 [get_ports {SEGMENT[3]}]
set_property PACKAGE_PIN W20 [get_ports {SEGMENT[4]}]
set_property PACKAGE_PIN AC24 [get_ports {SEGMENT[5]}]
set_property PACKAGE_PIN AC23 [get_ports {SEGMENT[6]}]
set_property PACKAGE_PIN AA22 [get_ports {SEGMENT[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {AN[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {AN[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {AN[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {AN[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[7]}]









top2:



`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/05/30 13:41:31
// Design Name: 
// Module Name: top2
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


module top2(
    input [15:0] SW,
    input clk,
    output BTN_X,
    output[3:0]AN,
    output[7:0] SEGMENT
    );
    wire[31:0] clk_div;
    clkdiv p0(
        .clk(clk),
        .rst(1'b0),
        .div_res(clk_div)
    );
    wire[3:0] A_in,A_out,B_in,B_out,C_in,C_out;
    wire Load_A,Load_B,Load_C,a,b;
    wire [3:0] A,B,main;
    MyLoad p1(
        .clk(clk),
        .clk_1ms(clk_div[17]),
        .btn_in(SW[2]),
        .Load_out(Load_A)
    );
    MyRegister4b p2(
        .load(Load_A),
        .in(A_in),
        .out(A_out)
    );
    MyLoad p3(
        .clk(clk),
        .clk_1ms(clk_div[17]),
        .btn_in(SW[3]),
        .Load_out(Load_B)
    );
    MyRegister4b p4(
        .load(Load_B),
        .in(B_in),
        .out(B_out)
    );
    MyLoad p5(
        .clk(clk),
        .clk_1ms(clk_div[17]),
        .btn_in(SW[4]),
        .Load_out(Load_C)
    );
    MyRegister4b p6(
        .load(Load_C),
        .in(C_in),
        .out(C_out)
    );
    assign C_in=SW[15]?4'b0000:main;
    Mux4to1b4 p7(
        .D0(A_out),
        .D1(B_out),
        .D2(C_out),
        .D3(4'b0000),
        .S(SW[8:7]),
        .Y(main)
    );
    assign BTN_X = 1'b0;
    AddSub4b p8(
        .A(A_out),
        .B(4'b0001),
        .Ctrl(SW[0]),
        .S(A),
        .Cout(a)
    );
    assign A_in=(SW[15]==1'b0)?A:main;
    AddSub4b p9(
        .A(B_out),
        .B(4'b0001),
        .Ctrl(SW[1]),
        .S(B),
        .Cout(b)
    );
    assign B_in=(SW[15]==1'b0)?B:main;
    Displaynumber p10(
        .clk(clk),
        .rst(1'b0),
        .hexs({A_out,B_out,C_out,main}),
        .points(4'b0000),
        .LEs(4'b0),
        .AN(AN),
        .SEGMENT(SEGMENT)
    );
endmodule




set_property PACKAGE_PIN AC18 [get_ports clk]
set_property IOSTANDARD LVCMOS18 [get_ports clk]

create_clock -period 10.000 -name clk [get_ports "clk"]
set_property PACKAGE_PIN W16 [get_ports BTN_X]
set_property IOSTANDARD LVCMOS18 [get_ports BTN_X]
set_property PACKAGE_PIN AD21 [get_ports {AN[0]}]
set_property PACKAGE_PIN AC21 [get_ports {AN[1]}]
set_property PACKAGE_PIN AB21 [get_ports {AN[2]}]
set_property PACKAGE_PIN AC22 [get_ports {AN[3]}]
set_property PACKAGE_PIN AB22 [get_ports {SEGMENT[0]}]
set_property PACKAGE_PIN AD24 [get_ports {SEGMENT[1]}]
set_property PACKAGE_PIN AD23 [get_ports {SEGMENT[2]}]
set_property PACKAGE_PIN Y21 [get_ports {SEGMENT[3]}]
set_property PACKAGE_PIN W20 [get_ports {SEGMENT[4]}]
set_property PACKAGE_PIN AC24 [get_ports {SEGMENT[5]}]
set_property PACKAGE_PIN AC23 [get_ports {SEGMENT[6]}]
set_property PACKAGE_PIN AA22 [get_ports {SEGMENT[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {AN[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {AN[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {AN[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {AN[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SEGMENT[7]}]
set_property PACKAGE_PIN AA13 [get_ports {SW[15]}]
set_property IOSTANDARD LVCMOS15 [get_ports {SW[15]}]
set_property PACKAGE_PIN AA10 [get_ports {SW[7]}]
set_property PACKAGE_PIN AB10 [get_ports {SW[8]}]
set_property IOSTANDARD LVCMOS15 [get_ports {SW[7]}]
set_property IOSTANDARD LVCMOS15 [get_ports {SW[8]}]
set_property PACKAGE_PIN AA12 [get_ports {SW[0]}]
set_property IOSTANDARD LVCMOS15 [get_ports {SW[0]}]
set_property PACKAGE_PIN Y13 [get_ports {SW[1]}]
set_property IOSTANDARD LVCMOS15 [get_ports {SW[1]}]
set_property PACKAGE_PIN V19 [get_ports SW[2]]
set_property IOSTANDARD LVCMOS18 [get_ports SW[2]]
set_property PACKAGE_PIN V14 [get_ports SW[3]]
set_property IOSTANDARD LVCMOS18 [get_ports SW[3]]
set_property PACKAGE_PIN W14 [get_ports SW[4]]
set_property IOSTANDARD LVCMOS18 [get_ports SW[4]]









