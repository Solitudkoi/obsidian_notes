![[Pasted image 20250529171306.png]]
![[Pasted image 20250530145049.png]]
![[Pasted image 20250530161206.png]]



40103533













`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/05/30 14:51:25
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
    input [15:0] SW,
    output[3:0] AN,
    output[7:0] SEGMENT,
    output LEDCLK,
    output LEDCLR,
    output  LEDEN,
    output  LEDDT,
    output BTN_X
    );
    assign BTN_X=0;
    wire [15:0] data;
    wire finish;
    wire [31:0] clkdiv;
    wire [3:0] btnout;
    
    clkdiv clkdiv_inst (
        .clk(clk),
        .rst(1'b0),
        .div_res(clkdiv)
    );
    
    genvar i;
    generate
    for(i=0;i<4;i=i+1)begin: debounce_gen
        pbdebounce pbdebounce_inst (
            .clk(clkdiv[17]),
            .button(SW[i]),
            .pbreg(btnout[i])
        );
    end
    endgenerate
    CreateNumber CreateNumber_inst (
        .btn(btnout[3:0]),
        .num(data)
    );
    LED_DRV LED_DRV_inst (
        .clk(clkdiv[25]),
        .load(SW[15]),
        .in(data),
        .LED_D0(LEDDT),
        .finish(finish)
    );
    DisplayNumber DisplayNumber_inst (
        .clk(clk),
        .rst(1'b0),
        .hexs(data),
        .points(4'b0000),
        .LEs(4'b0000),
        .AN(AN),
        .SEGMENT(SEGMENT)
    );
    assign LEDCLK=clkdiv[25]&finish;
    assign LEDCLR=1'b1;
    assign LEDEN=1'b1;
endmodule