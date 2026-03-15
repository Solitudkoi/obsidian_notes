```
module CSSTE(
    input clk_100mhz,
    input RSTN,
    input [3:0] BTN_y,
    input [15:0] SW,
    output [3:0] Blue,
    output [3:0] Green,
    output [3:0] Red,
    output HSYNC,
    output VSYNC,
    output [15:0] LED_out,
    output [7:0] AN,
    output [7:0] segment
);

    //================ 时钟与复位 ====================
    wire [31:0] clkdiv;
    wire Clk_CPU;
    wire rst;

    //================ 按键去抖 ======================
    wire [3:0] BTN_OK;
    wire [15:0] SW_OK;
    wire [3:0] pulse_out;
    wire [4:0] Key_out;
    wire Key_ready;
    wire CR;
    wire readn;
    wire [4:0] Key_x;  // 声明信号但不连接外部

    SAnti_jitter U9 (
        .clk(clk_100mhz),
        .RSTN(RSTN),
        .readn(readn),
        .Key_y(BTN_y),
        //.Key_x(5'b11111),
        .Key_x(Key_x),     // 不使用矩阵键盘，仅占位
        .SW(SW),
        .Key_out(Key_out),
        .Key_ready(Key_ready),
        .pulse_out(pulse_out),
        .BTN_OK(BTN_OK),
        .SW_OK(SW_OK),
        .CR(CR),
        .rst(rst)
    );

    //================ 时钟分频器 ====================
    clk_div U8 (
        .clk(clk_100mhz),
        .rst(rst),
        .SW2(SW_OK[2]),
        .SW8(SW_OK[8]),
        .STEP(pulse_out[0]),
        .clkdiv(clkdiv),
        .Clk_CPU(Clk_CPU)
    );

    //================ CPU ====================
    wire [31:0] Addr_out, Data_in, Data_out, PC_out, Inst_in;
    wire MemRW;

    SCPU U1 (
        .clk(Clk_CPU),
        .rst(rst),
        .Addr_out(Addr_out),
        .Data_in(Data_in),
        .Data_out(Data_out),
        .MemRW(MemRW),
        .MIO_ready(1'b0),
        .PC_out(PC_out),
        .inst_in(Inst_in)
    );

    //================ 指令 ROM ====================
    ROM_D_0 U2 (
        .a(PC_out[11:2]),   // PC[11:2]
        .spo(Inst_in)
    );

    //================ 数据 RAM ====================
    wire [9:0] ram_addr;
    wire [31:0] ram_data_in, RAM_B_0_douta;
    wire U4_data_ram_we;

    RAM_B U3 (
        .clka(~clk_100mhz),
        .wea(U4_data_ram_we),
        .addra(ram_addr),
        .dina(ram_data_in),
        .douta(RAM_B_0_douta)
    );

    //================ MIO 总线 ====================
    wire [31:0] counter_out, Peripheral_in;
    wire counter_we;
    wire [15:0] LED_out_wire;

    MIO_BUS U4 (
        .clk(clk_100mhz),
        .rst(rst),
        .BTN(BTN_OK),
        .SW(SW_OK),
        .mem_w(MemRW),
        .Cpu_data2bus(Data_out),
        .addr_bus(Addr_out),
        .ram_data_out(RAM_B_0_douta),
        .led_out(LED_out_wire),
        .counter_out(counter_out),
        .Cpu_data4bus(Data_in),
        .ram_data_in(ram_data_in),
        .ram_addr(ram_addr),
        .data_ram_we(U4_data_ram_we),
        .counter_we(counter_we),
        .Peripheral_in(Peripheral_in)
    );

    assign LED_out = LED_out_wire;

    //================ 多通道显示控制 ====================
    wire [31:0] Disp_num;
    wire [7:0] point_out, LE_out;

    Multi_8CH32 U5 (
        .clk(clk_100mhz),
        .rst(rst),
        .EN(1'b1),
        .Test(SW_OK[7:5]),
        //.point_in({32{1'b0}}),
        //.LES({32{1'b0}}),
        .point_in(32'hFFFFFFFF),   // ✅ 改为 32 位
        .LES(32'h00000000),        // ✅ 改为 32 位
        .Data0(PC_out),
        .data1(Addr_out),
        .data2(Data_out),
        .data3(Data_in),
        .data4(Peripheral_in),
        .data5(Inst_in),
        .data6(counter_out),
        .data7(clkdiv),
        .point_out(point_out),
        .LE_out(LE_out),
        .Disp_num(Disp_num)
    );

    //================ 数码管显示 ====================
    Seg7_Dev_0 U6 (
        .disp_num(Disp_num),
        .point(point_out),
        .les(LE_out),
        .scan(clkdiv[18:16]),
        .AN(AN),
        .segment(segment)
    );

    //================ LED SPI 模块（可选） ====================
    wire [1:0] counter_set;

    SPIO U7 (
        .clk(clk_100mhz),
        .rst(rst),
        .Start(1'b0),
        .EN(1'b1),
        .P_Data({16'b0, LED_out_wire}),
        //.counter_set(2'b00),
        .counter_set(counter_set), // ✅ 正确连接
        .LED_out(LED_out_wire),
        .led_clk(),
        .led_sout(),
        .led_clrn(),
        .LED_PEN(),
        .GPIOf0()
    );

    //================ 计数器模块 ====================
    wire counter0_OUT, counter1_OUT, counter2_OUT;

    Counter_x U10 (
        .clk(clk_100mhz),
        .rst(rst),
        .clk0(clkdiv[6]),
        .clk1(clkdiv[9]),
        .clk2(clkdiv[11]),
        .counter_we(counter_we),
        .counter_val(Data_out),
        .counter_ch(Addr_out[1:0]),
        .counter0_OUT(counter0_OUT),
        .counter1_OUT(counter1_OUT),
        .counter2_OUT(counter2_OUT),
        .counter_out(counter_out)
    );

    //================ VGA 显示模块 ====================
    VGA U11 (
        .clk_25m(clkdiv[1]),
        .clk_100m(clk_100mhz),
        .rst(rst),
        .pc(PC_out),
        .inst(Inst_in),
        .alu_res(Addr_out),
        .mem_wen(MemRW),
        .dmem_o_data(Data_out),
        .dmem_i_data(Data_in),
        .dmem_addr(Addr_out),
        .hs(HSYNC),
        .vs(VSYNC),
        .vga_r(Red),
        .vga_g(Green),
        .vga_b(Blue)
    );

endmodule


```