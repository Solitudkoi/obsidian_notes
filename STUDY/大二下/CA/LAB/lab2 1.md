```

`timescale 1ns / 1ps

module ExceptionUnit(
    input              clk,
    input              rst,

    // CSR instruction info from MEM stage
    input              csr_rw_in,
    input      [1:0]   csr_wsc_mode_in,   // funct3[1:0]: 01=rw, 10=rs, 11=rc
    input              csr_w_imm_mux,     // 0: rs1 data, 1: zimm
    input      [11:0]  csr_rw_addr_in,
    input      [31:0]  csr_w_data_reg,
    input      [4:0]   csr_w_data_imm,
    output reg [31:0]  csr_r_data_out,

    // exception / interrupt from WB stage
    input              interrupt,
    input              illegal_inst,
    input              ecall_m,
    input              l_access_fault,
    input              s_access_fault,

    // mret from MEM stage
    input              mret,

    // PC info
    input      [31:0]  epc_cur,   // current instruction PC (for exception)
    input      [31:0]  epc_next,  // next instruction PC (for interrupt)

    // redirect control
    output reg [31:0]  PC_redirect,
    output reg         redirect_mux,

    // pipeline flush
    output wire        reg_FD_flush,
    output wire        reg_DE_flush,
    output wire        reg_EM_flush,
    output wire        reg_MW_flush,

    // cancel register writeback for faulting instruction
    output wire        RegWrite_cancel
);

    // ----------------------------
    // CSR registers (only M-mode)
    // ----------------------------
    reg [31:0] mstatus;
    reg [31:0] mtvec;
    reg [31:0] mepc;
    reg [31:0] mcause;

    // temp regs for state machine
    reg [31:0] epc_reg;
    reg [31:0] cause_reg;

    // CSR addresses
    localparam CSR_MSTATUS = 12'h300;
    localparam CSR_MTVEC   = 12'h305;
    localparam CSR_MEPC    = 12'h341;
    localparam CSR_MCAUSE  = 12'h342;

    // state machine
    localparam STATE_IDLE   = 3'd0;
    localparam STATE_MEPC   = 3'd1;
    localparam STATE_MCAUSE = 3'd2;
    localparam STATE_MRET   = 3'd3;

    reg [2:0] state, next_state;

    wire trap_req_wb;
    wire sync_exception;

    assign trap_req_wb    = interrupt | illegal_inst | ecall_m | l_access_fault | s_access_fault;
    assign sync_exception = illegal_inst | ecall_m | l_access_fault | s_access_fault;

    // ----------------------------
    // CSR read mux
    // ----------------------------
    always @(*) begin
        case (csr_rw_addr_in)
            CSR_MSTATUS: csr_r_data_out = mstatus;
            CSR_MTVEC  : csr_r_data_out = mtvec;
            CSR_MEPC   : csr_r_data_out = mepc;
            CSR_MCAUSE : csr_r_data_out = mcause;
            default    : csr_r_data_out = 32'h00000000;
        endcase
    end

    // ----------------------------
    // next-state logic
    // ----------------------------
    always @(*) begin
        next_state = state;
        case (state)
            STATE_IDLE: begin
                if (trap_req_wb)
                    next_state = STATE_MEPC;
                else
                    next_state = STATE_IDLE;
            end

            STATE_MEPC: begin
                next_state = STATE_MCAUSE;
            end

            STATE_MCAUSE: begin
                next_state = STATE_IDLE;
            end

            default: begin
                next_state = STATE_IDLE;
            end
        endcase
    end
    // ----------------------------
    // redirect control
    // trap: redirect in STATE_MCAUSE
    // mret: redirect in STATE_MRET
    // ----------------------------
    always @(*) begin
        redirect_mux = 1'b0;
        PC_redirect  = 32'h00000000;

        // trap: 第二步重定向到 mtvec
        if (state == STATE_MEPC) begin
            redirect_mux = 1'b1;
            PC_redirect  = {mtvec[31:2], 2'b00};
        end
        // mret: 到 MEM 后立即准备重定向
        else if (mret) begin
            redirect_mux = 1'b1;
            PC_redirect  = mepc;
        end
    end

    // ----------------------------
    // flush control
    // ----------------------------
    assign reg_FD_flush = (state == STATE_MEPC) | mret;
    assign reg_DE_flush = (state == STATE_MEPC) | mret;
    assign reg_EM_flush = (state == STATE_MEPC) | mret;
    assign reg_MW_flush = 1'b0;

    // cancel WB only for synchronous exception when entering exception flow
    assign RegWrite_cancel = (state == STATE_MEPC) & sync_exception;

    // helpers
    reg [31:0] csr_old;
    reg [31:0] csr_wdata;
    reg [31:0] csr_src;

    // ----------------------------
    // sequential logic
    // ----------------------------
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            state    <= STATE_IDLE;
            mstatus  <= 32'h0000_0008;
            mtvec    <= 32'h0000_0000;
            mepc     <= 32'h0000_0000;
            mcause   <= 32'h0000_0000;
            epc_reg  <= 32'h0000_0000;
            cause_reg<= 32'h0000_0000;
        end
        else begin
            state <= next_state;

            case (state)
                // ----------------------------
                // idle:
                // - normal CSR write
                // - latch trap info
                // - restore mstatus for mret
                // ----------------------------
                STATE_IDLE: begin
                    if (trap_req_wb) begin
                        // record epc
                        if (interrupt)
                            epc_reg <= epc_next;
                        else
                            epc_reg <= epc_cur;

                        // record cause
                        if (interrupt)
                            cause_reg <= 32'h8000_000b;
                        else if (illegal_inst)
                            cause_reg <= 32'd2;
                        else if (ecall_m)
                            cause_reg <= 32'd11;
                        else if (l_access_fault)
                            cause_reg <= 32'd5;
                        else if (s_access_fault)
                            cause_reg <= 32'd7;

                        // update mstatus: MPIE <= MIE, MIE <= 0
                        mstatus[7] <= mstatus[3];
                        mstatus[3] <= 1'b0;
                    end
                    else if (mret) begin
                        // restore mstatus: MIE <= MPIE, MPIE <= 1
                        mstatus[3] <= mstatus[7];
                        mstatus[7] <= 1'b1;
                    end
                    else if (csr_rw_in) begin
                        csr_src = csr_w_imm_mux ? {27'b0, csr_w_data_imm} : csr_w_data_reg;

                        case (csr_rw_addr_in)
                            CSR_MSTATUS: begin
                                csr_old = mstatus;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mstatus <= csr_wdata;
                            end

                            CSR_MTVEC: begin
                                csr_old = mtvec;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mtvec <= csr_wdata;
                            end

                            CSR_MEPC: begin
                                csr_old = mepc;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mepc <= csr_wdata;
                            end

                            CSR_MCAUSE: begin
                                csr_old = mcause;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mcause <= csr_wdata;
                            end

                            default: begin
                            end
                        endcase
                    end
                end

                // ----------------------------
                // write mepc in second step
                // ----------------------------
                STATE_MEPC: begin
                    mepc <= epc_reg;
                end

                // ----------------------------
                // write mcause in third step
                // ----------------------------
                STATE_MCAUSE: begin
                    mcause <= cause_reg;
                end

                // ----------------------------
                // mret redirect step
                // ----------------------------
                STATE_MRET: begin
                    // no extra write here
                end

                default: begin
                end
            endcase
        end
    end

endmodule

```

```



`timescale 1ns / 1ps

module RV32core(
        input debug_en,
        input debug_step,
        input [6:0] debug_addr,
        output [31:0] debug_data,
        input clk,
        input rst,
        input interrupter
    );

    wire debug_clk;
    debug_clk clock(.clk(clk), .debug_en(debug_en), .debug_step(debug_step), .debug_clk(debug_clk));

    wire Branch_ctrl, JALR, RegWrite_ctrl, mem_w_ctrl, mem_r_ctrl;
    wire ALUSrc_A_ctrl, ALUSrc_B_ctrl, DatatoReg_ctrl, rs1use_ctrl, rs2use_ctrl;
    wire MRET, csr_rw_ctrl, csr_w_imm_mux_ctrl;
    wire [1:0] hazard_optype_ctrl, exp_vector_ctrl;
    wire [2:0] ImmSel_ctrl, cmp_ctrl;
    wire [3:0] ALUControl_ctrl;

    wire forward_ctrl_ls;
    wire [1:0] forward_ctrl_A, forward_ctrl_B;

    wire PC_EN_IF;
    wire [31:0] PC_IF, next_PC_IF, PC_4_IF, inst_IF, final_PC_IF;

    wire reg_FD_stall, reg_FD_flush, isFlushed_ID, cmp_res_ID;
    wire [31:0] jump_PC_ID, PC_ID, inst_ID, Debug_regs, rs1_data_reg, rs2_data_reg;
    wire [31:0] Imm_out_ID, rs1_data_ID, rs2_data_ID, addA_ID;

    wire reg_DE_flush, isFlushed_EXE, RegWrite_EXE, mem_w_EXE, mem_r_EXE;
    wire ALUSrc_A_EXE, ALUSrc_B_EXE, ALUzero_EXE, ALUoverflow_EXE, DatatoReg_EXE;
    wire csr_rw_EXE, csr_w_imm_mux_EXE, mret_EXE;
    wire [1:0] exp_vector_EXE;
    wire [2:0] u_b_h_w_EXE;
    wire [3:0] ALUControl_EXE;
    wire [4:0] rs1_EXE, rs2_EXE, rd_EXE;
    wire [31:0] ALUout_EXE, PC_EXE, inst_EXE, rs1_data_EXE, rs2_data_EXE, Imm_EXE;
    wire [31:0] ALUA_EXE, ALUB_EXE, Dataout_EXE;

    wire redirect_mux_exp, reg_FD_flush_exp, reg_DE_flush_exp;
    wire reg_EM_flush_exp, reg_MW_flush_exp, RegWrite_cancel_exp;
    wire [31:0] PC_redirect_exp;

    wire isFlushed_MEM, RegWrite_MEM, DatatoReg_MEM, mem_w_MEM, mem_r_MEM;
    wire csr_rw_MEM, csr_w_imm_mux_MEM, mret_MEM, l_access_fault_MEM, s_access_fault_MEM;
    wire [1:0] exp_vector_MEM;
    wire [2:0] u_b_h_w_MEM;
    wire [4:0] rs1_MEM, rd_MEM;
    wire [31:0] ALUout_MEM, PC_MEM, inst_MEM, Dataout_MEM, Datain_MEM;
    wire [31:0] rs1_data_MEM, CSRout_MEM, RAMout_MEM;

    wire isFlushed_WB, RegWrite_WB, DatatoReg_WB;
    wire [3:0] exp_vector_WB;
    wire [4:0] rd_WB;
    wire [31:0] wt_data_WB, PC_WB, inst_WB, ALUout_WB, Datain_WB;

    wire [31:0] Addr;
    wire MWR;
    assign Addr = ALUout_MEM;
    assign MWR = mem_w_MEM;

    // IF
    REG32 REG_PC(.clk(debug_clk), .rst(rst), .CE(PC_EN_IF), .D(final_PC_IF), .Q(PC_IF));
    add_32 add_IF(.a(PC_IF), .b(32'd4), .c(PC_4_IF));
    MUX2T1_32 mux_IF(.I0(PC_4_IF), .I1(jump_PC_ID), .s(Branch_ctrl), .o(next_PC_IF));
    MUX2T1_32 redirectPC(.I0(next_PC_IF), .I1(PC_redirect_exp), .s(redirect_mux_exp), .o(final_PC_IF));
    ROM_D inst_rom(.a(PC_IF[8:2]), .spo(inst_IF));

    // ID
    REG_IF_ID reg_IF_ID(
        .clk(debug_clk), .rst(rst), .EN(1'b1), .Data_stall(reg_FD_stall),
        .flush(reg_FD_flush | reg_FD_flush_exp), .PCOUT(PC_IF), .IR(inst_IF),
        .IR_ID(inst_ID), .PCurrent_ID(PC_ID), .isFlushed(isFlushed_ID)
    );

    CtrlUnit ctrl(
        .inst(inst_ID), .cmp_res(cmp_res_ID), .Branch(Branch_ctrl),
        .ALUSrc_A(ALUSrc_A_ctrl), .ALUSrc_B(ALUSrc_B_ctrl), .DatatoReg(DatatoReg_ctrl),
        .RegWrite(RegWrite_ctrl), .mem_w(mem_w_ctrl), .mem_r(mem_r_ctrl),
        .rs1use(rs1use_ctrl), .rs2use(rs2use_ctrl), .hazard_optype(hazard_optype_ctrl),
        .ImmSel(ImmSel_ctrl), .cmp_ctrl(cmp_ctrl), .ALUControl(ALUControl_ctrl),
        .JALR(JALR), .MRET(MRET), .csr_rw(csr_rw_ctrl),
        .csr_w_imm_mux(csr_w_imm_mux_ctrl), .exp_vector(exp_vector_ctrl)
    );

    Regs register(
        .clk(debug_clk), .rst(rst), .L_S(RegWrite_WB & ~RegWrite_cancel_exp),
        .R_addr_A(inst_ID[19:15]), .R_addr_B(inst_ID[24:20]),
        .rdata_A(rs1_data_reg), .rdata_B(rs2_data_reg), .Wt_addr(rd_WB), .Wt_data(wt_data_WB),
        .Debug_addr(debug_addr[4:0]), .Debug_regs(Debug_regs)
    );

    ImmGen imm_gen(.ImmSel(ImmSel_ctrl), .inst_field(inst_ID), .Imm_out(Imm_out_ID));

    MUX4T1_32 mux_forward_A(.I0(rs1_data_reg), .I1(ALUout_EXE), .I2(ALUout_MEM), .I3(Datain_MEM), .s(forward_ctrl_A), .o(rs1_data_ID));
    MUX4T1_32 mux_forward_B(.I0(rs2_data_reg), .I1(ALUout_EXE), .I2(ALUout_MEM), .I3(Datain_MEM), .s(forward_ctrl_B), .o(rs2_data_ID));
    MUX2T1_32 mux_branch_ID(.I0(PC_ID), .I1(rs1_data_ID), .s(JALR), .o(addA_ID));
    add_32 add_branch_ID(.a(addA_ID), .b(Imm_out_ID), .c(jump_PC_ID));
    cmp_32 cmp_ID(.a(rs1_data_ID), .b(rs2_data_ID), .ctrl(cmp_ctrl), .c(cmp_res_ID));

    HazardDetectionUnit hazard_unit(
        .clk(debug_clk), .Branch_ID(Branch_ctrl), .rs1use_ID(rs1use_ctrl), .rs2use_ID(rs2use_ctrl),
        .hazard_optype_ID(hazard_optype_ctrl), .rd_EXE(rd_EXE), .rd_MEM(rd_MEM),
        .rs1_ID(inst_ID[19:15]), .rs2_ID(inst_ID[24:20]), .rs2_EXE(rs2_EXE),
        .PC_EN_IF(PC_EN_IF), .reg_FD_stall(reg_FD_stall), .reg_FD_flush(reg_FD_flush),
        .reg_DE_flush(reg_DE_flush), .forward_ctrl_ls(forward_ctrl_ls),
        .forward_ctrl_A(forward_ctrl_A), .forward_ctrl_B(forward_ctrl_B)
    );

    // EX
    REG_ID_EX reg_ID_EX(
        .clk(debug_clk), .rst(rst), .EN(1'b1), .flush(reg_DE_flush | reg_DE_flush_exp | isFlushed_ID),
        .IR_ID(inst_ID), .PCurrent_ID(PC_ID), .rs1_addr(inst_ID[19:15]), .rs2_addr(inst_ID[24:20]),
        .rs1_data(rs1_data_ID), .rs2_data(rs2_data_ID), .Imm32(Imm_out_ID), .rd_addr(inst_ID[11:7]),
        .ALUSrc_A(ALUSrc_A_ctrl), .ALUSrc_B(ALUSrc_B_ctrl), .ALUC(ALUControl_ctrl),
        .DatatoReg(DatatoReg_ctrl), .RegWrite(RegWrite_ctrl), .WR(mem_w_ctrl),
        .u_b_h_w(inst_ID[14:12]), .mem_r(mem_r_ctrl), .csr_rw(csr_rw_ctrl),
        .csr_w_imm_mux(csr_w_imm_mux_ctrl), .mret(MRET), .exp_vector(exp_vector_ctrl),
        .PCurrent_EX(PC_EXE), .IR_EX(inst_EXE), .rs1_EX(rs1_EXE), .rs2_EX(rs2_EXE),
        .A_EX(rs1_data_EXE), .B_EX(rs2_data_EXE), .Imm32_EX(Imm_EXE), .rd_EX(rd_EXE),
        .ALUSrc_A_EX(ALUSrc_A_EXE), .ALUSrc_B_EX(ALUSrc_B_EXE), .ALUC_EX(ALUControl_EXE),
        .DatatoReg_EX(DatatoReg_EXE), .RegWrite_EX(RegWrite_EXE), .WR_EX(mem_w_EXE),
        .u_b_h_w_EX(u_b_h_w_EXE), .mem_r_EX(mem_r_EXE), .isFlushed(isFlushed_EXE),
        .csr_rw_EX(csr_rw_EXE), .csr_w_imm_mux_EX(csr_w_imm_mux_EXE), .mret_EX(mret_EXE),
        .exp_vector_EX(exp_vector_EXE)
    );

    MUX2T1_32 mux_A_EXE(.I0(rs1_data_EXE), .I1(PC_EXE), .s(ALUSrc_A_EXE), .o(ALUA_EXE));
    MUX2T1_32 mux_B_EXE(.I0(rs2_data_EXE), .I1(Imm_EXE), .s(ALUSrc_B_EXE), .o(ALUB_EXE));
    ALU alu(.A(ALUA_EXE), .B(ALUB_EXE), .Control(ALUControl_EXE), .res(ALUout_EXE), .zero(ALUzero_EXE), .overflow(ALUoverflow_EXE));
    MUX2T1_32 mux_forward_EXE(.I0(rs2_data_EXE), .I1(Datain_MEM), .s(forward_ctrl_ls), .o(Dataout_EXE));

    // MEM
    REG_EX_MEM reg_EXE_MEM(
        .clk(debug_clk), .rst(rst), .EN(1'b1), .flush(reg_EM_flush_exp | isFlushed_EXE),
        .IR_EX(inst_EXE), .PCurrent_EX(PC_EXE), .ALUO_EX(ALUout_EXE), .B_EX(Dataout_EXE),
        .rd_EX(rd_EXE), .rs1_EX(rs1_EXE), .rs1_data_EX(rs1_data_EXE), .DatatoReg_EX(DatatoReg_EXE),
        .RegWrite_EX(RegWrite_EXE), .WR_EX(mem_w_EXE), .u_b_h_w_EX(u_b_h_w_EXE), .mem_r_EX(mem_r_EXE),
        .csr_rw_EX(csr_rw_EXE), .csr_w_imm_mux_EX(csr_w_imm_mux_EXE), .mret_EX(mret_EXE),
        .exp_vector_EX(exp_vector_EXE),
        .PCurrent_MEM(PC_MEM), .IR_MEM(inst_MEM), .ALUO_MEM(ALUout_MEM), .Datao_MEM(Dataout_MEM),
        .rd_MEM(rd_MEM), .rs1_MEM(rs1_MEM), .rs1_data_MEM(rs1_data_MEM), .DatatoReg_MEM(DatatoReg_MEM),
        .RegWrite_MEM(RegWrite_MEM), .WR_MEM(mem_w_MEM), .u_b_h_w_MEM(u_b_h_w_MEM), .mem_r_MEM(mem_r_MEM),
        .isFlushed(isFlushed_MEM), .csr_rw_MEM(csr_rw_MEM), .csr_w_imm_mux_MEM(csr_w_imm_mux_MEM),
        .mret_MEM(mret_MEM), .exp_vector_MEM(exp_vector_MEM)
    );

    RAM_B data_ram(
        .addra(ALUout_MEM), .clka(debug_clk), .dina(Dataout_MEM), .wea(mem_w_MEM), .rea(mem_r_MEM),
        .douta(RAMout_MEM), .mem_u_b_h_w(u_b_h_w_MEM),
        .l_access_fault(l_access_fault_MEM), .s_access_fault(s_access_fault_MEM)
    );

    wire [31:0] csr_w_data_reg_MEM;
    assign csr_w_data_reg_MEM =
                            (RegWrite_WB && (rd_WB != 5'd0) && (rd_WB == rs1_MEM)) ? wt_data_WB :
                            rs1_data_MEM;

    ExceptionUnit exp_unit(.clk(debug_clk),.rst(rst),.csr_rw_in(csr_rw_MEM),.csr_wsc_mode_in(inst_MEM[13:12]),
        .csr_w_imm_mux(csr_w_imm_mux_MEM),.csr_rw_addr_in(inst_MEM[31:20]),
        .csr_w_data_reg(csr_w_data_reg_MEM),.csr_w_data_imm(inst_MEM[19:15]),
        .csr_r_data_out(CSRout_MEM),
        .interrupt(interrupter),
        .illegal_inst(~isFlushed_WB & exp_vector_WB[3]),
        .ecall_m(~isFlushed_WB & exp_vector_WB[2]),
        .l_access_fault(~isFlushed_WB & exp_vector_WB[1]),
        .s_access_fault(~isFlushed_WB & exp_vector_WB[0]),
        .mret(mret_MEM), .epc_cur(PC_WB),
        .epc_next(~isFlushed_MEM ? PC_MEM : (~isFlushed_EXE ? PC_EXE : (~isFlushed_ID ? PC_ID : PC_IF))),
        .PC_redirect(PC_redirect_exp), .redirect_mux(redirect_mux_exp),
        .reg_FD_flush(reg_FD_flush_exp), .reg_DE_flush(reg_DE_flush_exp),
        .reg_EM_flush(reg_EM_flush_exp), .reg_MW_flush(reg_MW_flush_exp),
        .RegWrite_cancel(RegWrite_cancel_exp)
    );

    MUX2T1_32 mux_csrout(.I0(RAMout_MEM), .I1(CSRout_MEM), .s(csr_rw_MEM), .o(Datain_MEM));

    // WB
    REG_MEM_WB reg_MEM_WB(
        .clk(debug_clk), .rst(rst), .EN(1'b1), .flush(reg_MW_flush_exp | isFlushed_MEM),
        .IR_MEM(inst_MEM), .PCurrent_MEM(PC_MEM), .ALUO_MEM(ALUout_MEM), .Datai(Datain_MEM),
        .rd_MEM(rd_MEM), .DatatoReg_MEM(DatatoReg_MEM), .RegWrite_MEM(RegWrite_MEM),
        .exp_vector_MEM({exp_vector_MEM, l_access_fault_MEM, s_access_fault_MEM}),
        .PCurrent_WB(PC_WB), .IR_WB(inst_WB), .ALUO_WB(ALUout_WB), .MDR_WB(Datain_WB),
        .rd_WB(rd_WB), .DatatoReg_WB(DatatoReg_WB), .RegWrite_WB(RegWrite_WB),
        .isFlushed(isFlushed_WB), .exp_vector_WB(exp_vector_WB)
    );

    MUX2T1_32 mux_WB(.I0(ALUout_WB), .I1(Datain_WB), .s(DatatoReg_WB), .o(wt_data_WB));

    wire [31:0] Test_signal;
    assign debug_data = debug_addr[5] ? Test_signal : Debug_regs;

    CPUTEST U1_3(
        .PC_IF(PC_IF), .PC_ID(PC_ID), .PC_EXE(PC_EXE), .PC_MEM(PC_MEM), .PC_WB(PC_WB),
        .PC_next_IF(next_PC_IF), .PCJump(jump_PC_ID),
        .inst_IF(inst_IF), .inst_ID(inst_ID), .inst_EXE(inst_EXE), .inst_MEM(inst_MEM), .inst_WB(inst_WB),
        .PCEN(PC_EN_IF), .Branch(Branch_ctrl), .PCSource(Branch_ctrl),
        .RS1DATA(rs1_data_reg), .RS2DATA(rs2_data_reg), .Imm32(Imm_out_ID), .ImmSel(ImmSel_ctrl),
        .ALUC(ALUControl_ctrl), .ALUSrc_A(ALUSrc_A_ctrl), .ALUSrc_B(ALUSrc_B_ctrl),
        .A(ALUA_EXE), .B(ALUB_EXE), .ALU_out(ALUout_MEM), .Datai(Datain_MEM), .Datao(Dataout_MEM),
        .Addr(Addr), .WR(MWR), .MIO(mem_r_MEM), .WDATA(wt_data_WB), .DatatoReg(DatatoReg_WB), .RegWrite(RegWrite_WB),
        .data_hazard(reg_FD_stall), .control_hazard(Branch_ctrl),
        .exp_sig({csr_rw_MEM,2'b0,csr_w_imm_mux_MEM,
                  isFlushed_ID,isFlushed_EXE,isFlushed_MEM,isFlushed_WB,
                  interrupter,3'b0,exp_vector_WB,
                  4'b0,
                  3'b0,redirect_mux_exp,
                  reg_FD_flush_exp,reg_DE_flush_exp,reg_EM_flush_exp,reg_MW_flush_exp,
                  3'b0,RegWrite_cancel_exp}),
        .Debug_addr(debug_addr[4:0]), .Test_signal(Test_signal)
    );

endmodule


```

```
`timescale 1ns / 1ps

module ExceptionUnit(
    input              clk,
    input              rst,

    // CSR instruction info from MEM stage
    input              csr_rw_in,
    input      [1:0]   csr_wsc_mode_in,   // funct3[1:0]: 01=rw, 10=rs, 11=rc
    input              csr_w_imm_mux,     // 0: rs1 data, 1: zimm
    input      [11:0]  csr_rw_addr_in,
    input      [31:0]  csr_w_data_reg,
    input      [4:0]   csr_w_data_imm,
    output reg [31:0]  csr_r_data_out,

    // exception / interrupt from WB stage
    input              interrupt,
    input              illegal_inst,
    input              ecall_m,
    input              l_access_fault,
    input              s_access_fault,

    // mret from MEM stage
    input              mret,

    // PC info
    input      [31:0]  epc_cur,   // current instruction PC (for exception)
    input      [31:0]  epc_next,  // next instruction PC (for interrupt)

    // redirect control
    output reg [31:0]  PC_redirect,
    output reg         redirect_mux,

    // pipeline flush
    output wire        reg_FD_flush,
    output wire        reg_DE_flush,
    output wire        reg_EM_flush,
    output wire        reg_MW_flush,

    // cancel register writeback for faulting instruction
    output wire        RegWrite_cancel
);

    // ----------------------------
    // CSR registers (only M-mode)
    // ----------------------------
    reg [31:0] mstatus;
    reg [31:0] mtvec;
    reg [31:0] mepc;
    reg [31:0] mcause;

    // temp regs for state machine
    reg [31:0] epc_reg;
    reg [31:0] cause_reg;

    // CSR addresses
    localparam CSR_MSTATUS = 12'h300;
    localparam CSR_MTVEC   = 12'h305;
    localparam CSR_MEPC    = 12'h341;
    localparam CSR_MCAUSE  = 12'h342;

    // state machine
    localparam STATE_IDLE   = 3'd0;
    localparam STATE_MEPC   = 3'd1;
    localparam STATE_MCAUSE = 3'd2;

    reg [2:0] state, next_state;

    wire interrupt_taken;
    wire trap_req_wb;
    wire sync_exception;

    // MIE (mstatus[3]) must gate interrupt acceptance.
    assign interrupt_taken = interrupt & mstatus[3] & (state == STATE_IDLE) & ~mret;
    //assign trap_req_wb     = interrupt_taken | illegal_inst | ecall_m | l_access_fault | s_access_fault;
    assign trap_req_wb = (interrupt & mstatus[3]) | illegal_inst | ecall_m | l_access_fault | s_access_fault;
    assign sync_exception  = illegal_inst | ecall_m | l_access_fault | s_access_fault;

    // ----------------------------
    // CSR read mux
    // ----------------------------
    always @(*) begin
        case (csr_rw_addr_in)
            CSR_MSTATUS: csr_r_data_out = mstatus;
            CSR_MTVEC  : csr_r_data_out = mtvec;
            CSR_MEPC   : csr_r_data_out = mepc;
            CSR_MCAUSE : csr_r_data_out = mcause;
            default    : csr_r_data_out = 32'h00000000;
        endcase
    end

    // ----------------------------
    // next-state logic
    // ----------------------------
    always @(*) begin
        next_state = state;
        case (state)
            STATE_IDLE: begin
                if (trap_req_wb)
                    next_state = STATE_MEPC;
                else
                    next_state = STATE_IDLE;
            end

            STATE_MEPC: begin
                next_state = STATE_MCAUSE;
            end

            STATE_MCAUSE: begin
                next_state = STATE_IDLE;
            end

            default: begin
                next_state = STATE_IDLE;
            end
        endcase
    end

    // ----------------------------
    // redirect control
    // trap: redirect in STATE_MEPC
    // mret: redirect in MEM when no trap sequence is ongoing
    // ----------------------------
    always @(*) begin
        redirect_mux = 1'b0;
        PC_redirect  = 32'h00000000;

        if (state == STATE_MEPC) begin
            redirect_mux = 1'b1;
            PC_redirect  = {mtvec[31:2], 2'b00};
        end
        else if (mret && (state == STATE_IDLE)) begin
            redirect_mux = 1'b1;
            PC_redirect  = mepc;
        end
    end

    // ----------------------------
    // flush control
    // ----------------------------
    assign reg_FD_flush = (state == STATE_MEPC) | (mret && (state == STATE_IDLE));
    assign reg_DE_flush = (state == STATE_MEPC) | (mret && (state == STATE_IDLE));
    assign reg_EM_flush = (state == STATE_MEPC) | (mret && (state == STATE_IDLE));
    assign reg_MW_flush = 1'b0;

    // cancel WB only for synchronous exception when entering exception flow
    assign RegWrite_cancel = (state == STATE_MEPC) & sync_exception;

    // helpers
    reg [31:0] csr_old;
    reg [31:0] csr_wdata;
    reg [31:0] csr_src;

    // ----------------------------
    // sequential logic
    // ----------------------------
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            state     <= STATE_IDLE;
            mstatus   <= 32'h0000_0008; // MIE = 1
            mtvec     <= 32'h0000_0000;
            mepc      <= 32'h0000_0000;
            mcause    <= 32'h0000_0000;
            epc_reg   <= 32'h0000_0000;
            cause_reg <= 32'h0000_0000;
        end
        else begin
            state <= next_state;

            case (state)
                // ----------------------------
                // idle:
                // - normal CSR write
                // - latch trap info
                // - restore mstatus for mret
                // ----------------------------
                STATE_IDLE: begin
                    if (trap_req_wb) begin
                        if (interrupt_taken)
                            epc_reg <= epc_next;
                        else if (ecall_m)
                            epc_reg <= epc_cur + 4;  // For ecall, set mepc to next instruction to avoid infinite loop
                        else
                            epc_reg <= epc_cur;

                        if (interrupt_taken)
                            cause_reg <= 32'h8000_000b;
                        else if (illegal_inst)
                            cause_reg <= 32'd2;
                        else if (ecall_m)
                            cause_reg <= 32'd11;
                        else if (l_access_fault)
                            cause_reg <= 32'd5;
                        else if (s_access_fault)
                            cause_reg <= 32'd7;

                        // enter trap: MPIE <= MIE, MIE <= 0
                        mstatus[7] <= mstatus[3];
                        mstatus[3] <= 1'b0;
                    end
                    else if (mret) begin
                        // exit trap: MIE <= MPIE, MPIE <= 1
                        mstatus[3] <= mstatus[7];
                        mstatus[7] <= 1'b1;
                    end
                    else if (csr_rw_in) begin
                        csr_src = csr_w_imm_mux ? {27'b0, csr_w_data_imm} : csr_w_data_reg;

                        case (csr_rw_addr_in)
                            CSR_MSTATUS: begin
                                csr_old = mstatus;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mstatus <= csr_wdata;
                            end

                            CSR_MTVEC: begin
                                csr_old = mtvec;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mtvec <= csr_wdata;
                            end

                            CSR_MEPC: begin
                                csr_old = mepc;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mepc <= csr_wdata;
                            end

                            CSR_MCAUSE: begin
                                csr_old = mcause;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mcause <= csr_wdata;
                            end

                            default: begin
                            end
                        endcase
                    end
                end

                // write mepc in second step
                STATE_MEPC: begin
                    mepc <= epc_reg;
                end

                // write mcause in third step
                STATE_MCAUSE: begin
                    mcause <= cause_reg;
                end

                default: begin
                end
            endcase
        end
    end

endmodule

```

```

`timescale 1ns / 1ps

module RV32core(
        input debug_en,
        input debug_step,
        input [6:0] debug_addr,
        output [31:0] debug_data,
        input clk,
        input rst,
        input interrupter
    );

    // Synchronize the external switch interrupt and compress it into a
    // single-cycle pulse, so one toggle of SW12 only triggers one trap.
    reg intr_sync_0, intr_sync_1, intr_sync_1_d;
    wire interrupt_pulse;

    wire debug_clk;
    debug_clk clock(.clk(clk), .debug_en(debug_en), .debug_step(debug_step), .debug_clk(debug_clk));

    always @(posedge debug_clk or posedge rst) begin
        if (rst) begin
            intr_sync_0   <= 1'b0;
            intr_sync_1   <= 1'b0;
            intr_sync_1_d <= 1'b0;
        end
        else begin
            intr_sync_0   <= interrupter;
            intr_sync_1   <= intr_sync_0;
            intr_sync_1_d <= intr_sync_1;
        end
    end

    assign interrupt_pulse = intr_sync_1 & ~intr_sync_1_d;

    wire Branch_ctrl, JALR, RegWrite_ctrl, mem_w_ctrl, mem_r_ctrl;
    wire ALUSrc_A_ctrl, ALUSrc_B_ctrl, DatatoReg_ctrl, rs1use_ctrl, rs2use_ctrl;
    wire MRET, csr_rw_ctrl, csr_w_imm_mux_ctrl;
    wire [1:0] hazard_optype_ctrl, exp_vector_ctrl;
    wire [2:0] ImmSel_ctrl, cmp_ctrl;
    wire [3:0] ALUControl_ctrl;

    wire forward_ctrl_ls;
    wire [1:0] forward_ctrl_A, forward_ctrl_B;

    wire PC_EN_IF;
    wire [31:0] PC_IF, next_PC_IF, PC_4_IF, inst_IF, final_PC_IF;

    wire reg_FD_stall, reg_FD_flush, isFlushed_ID, cmp_res_ID;
    wire [31:0] jump_PC_ID, PC_ID, inst_ID, Debug_regs, rs1_data_reg, rs2_data_reg;
    wire [31:0] Imm_out_ID, rs1_data_ID, rs2_data_ID, addA_ID;

    wire reg_DE_flush, isFlushed_EXE, RegWrite_EXE, mem_w_EXE, mem_r_EXE;
    wire ALUSrc_A_EXE, ALUSrc_B_EXE, ALUzero_EXE, ALUoverflow_EXE, DatatoReg_EXE;
    wire csr_rw_EXE, csr_w_imm_mux_EXE, mret_EXE;
    wire [1:0] exp_vector_EXE;
    wire [2:0] u_b_h_w_EXE;
    wire [3:0] ALUControl_EXE;
    wire [4:0] rs1_EXE, rs2_EXE, rd_EXE;
    wire [31:0] ALUout_EXE, PC_EXE, inst_EXE, rs1_data_EXE, rs2_data_EXE, Imm_EXE;
    wire [31:0] ALUA_EXE, ALUB_EXE, Dataout_EXE;

    wire redirect_mux_exp, reg_FD_flush_exp, reg_DE_flush_exp;
    wire reg_EM_flush_exp, reg_MW_flush_exp, RegWrite_cancel_exp;
    wire [31:0] PC_redirect_exp;

    wire isFlushed_MEM, RegWrite_MEM, DatatoReg_MEM, mem_w_MEM, mem_r_MEM;
    wire csr_rw_MEM, csr_w_imm_mux_MEM, mret_MEM, l_access_fault_MEM, s_access_fault_MEM;
    wire [1:0] exp_vector_MEM;
    wire [2:0] u_b_h_w_MEM;
    wire [4:0] rs1_MEM, rd_MEM;
    wire [31:0] ALUout_MEM, PC_MEM, inst_MEM, Dataout_MEM, Datain_MEM;
    wire [31:0] rs1_data_MEM, CSRout_MEM, RAMout_MEM;

    wire isFlushed_WB, RegWrite_WB, DatatoReg_WB;
    wire [3:0] exp_vector_WB;
    wire [4:0] rd_WB;
    wire [31:0] wt_data_WB, PC_WB, inst_WB, ALUout_WB, Datain_WB;

    wire [31:0] Addr;
    wire MWR;
    assign Addr = ALUout_MEM;
    assign MWR = mem_w_MEM;

    // IF
    REG32 REG_PC(.clk(debug_clk), .rst(rst), .CE(PC_EN_IF), .D(final_PC_IF), .Q(PC_IF));
    add_32 add_IF(.a(PC_IF), .b(32'd4), .c(PC_4_IF));
    MUX2T1_32 mux_IF(.I0(PC_4_IF), .I1(jump_PC_ID), .s(Branch_ctrl), .o(next_PC_IF));
    MUX2T1_32 redirectPC(.I0(next_PC_IF), .I1(PC_redirect_exp), .s(redirect_mux_exp), .o(final_PC_IF));
    ROM_D inst_rom(.a(PC_IF[8:2]), .spo(inst_IF));

    // ID
    REG_IF_ID reg_IF_ID(
        .clk(debug_clk), .rst(rst), .EN(1'b1), .Data_stall(reg_FD_stall),
        .flush(reg_FD_flush | reg_FD_flush_exp), .PCOUT(PC_IF), .IR(inst_IF),
        .IR_ID(inst_ID), .PCurrent_ID(PC_ID), .isFlushed(isFlushed_ID)
    );

    CtrlUnit ctrl(
        .inst(inst_ID), .cmp_res(cmp_res_ID), .Branch(Branch_ctrl),
        .ALUSrc_A(ALUSrc_A_ctrl), .ALUSrc_B(ALUSrc_B_ctrl), .DatatoReg(DatatoReg_ctrl),
        .RegWrite(RegWrite_ctrl), .mem_w(mem_w_ctrl), .mem_r(mem_r_ctrl),
        .rs1use(rs1use_ctrl), .rs2use(rs2use_ctrl), .hazard_optype(hazard_optype_ctrl),
        .ImmSel(ImmSel_ctrl), .cmp_ctrl(cmp_ctrl), .ALUControl(ALUControl_ctrl),
        .JALR(JALR), .MRET(MRET), .csr_rw(csr_rw_ctrl),
        .csr_w_imm_mux(csr_w_imm_mux_ctrl), .exp_vector(exp_vector_ctrl)
    );

    Regs register(
        .clk(debug_clk), .rst(rst), .L_S(RegWrite_WB & ~RegWrite_cancel_exp),
        .R_addr_A(inst_ID[19:15]), .R_addr_B(inst_ID[24:20]),
        .rdata_A(rs1_data_reg), .rdata_B(rs2_data_reg), .Wt_addr(rd_WB), .Wt_data(wt_data_WB),
        .Debug_addr(debug_addr[4:0]), .Debug_regs(Debug_regs)
    );

    ImmGen imm_gen(.ImmSel(ImmSel_ctrl), .inst_field(inst_ID), .Imm_out(Imm_out_ID));

    MUX4T1_32 mux_forward_A(.I0(rs1_data_reg), .I1(ALUout_EXE), .I2(ALUout_MEM), .I3(Datain_MEM), .s(forward_ctrl_A), .o(rs1_data_ID));
    MUX4T1_32 mux_forward_B(.I0(rs2_data_reg), .I1(ALUout_EXE), .I2(ALUout_MEM), .I3(Datain_MEM), .s(forward_ctrl_B), .o(rs2_data_ID));
    MUX2T1_32 mux_branch_ID(.I0(PC_ID), .I1(rs1_data_ID), .s(JALR), .o(addA_ID));
    add_32 add_branch_ID(.a(addA_ID), .b(Imm_out_ID), .c(jump_PC_ID));
    cmp_32 cmp_ID(.a(rs1_data_ID), .b(rs2_data_ID), .ctrl(cmp_ctrl), .c(cmp_res_ID));

    HazardDetectionUnit hazard_unit(
        .clk(debug_clk), .Branch_ID(Branch_ctrl), .rs1use_ID(rs1use_ctrl), .rs2use_ID(rs2use_ctrl),
        .hazard_optype_ID(hazard_optype_ctrl), .rd_EXE(rd_EXE), .rd_MEM(rd_MEM),
        .rs1_ID(inst_ID[19:15]), .rs2_ID(inst_ID[24:20]), .rs2_EXE(rs2_EXE),
        .PC_EN_IF(PC_EN_IF), .reg_FD_stall(reg_FD_stall), .reg_FD_flush(reg_FD_flush),
        .reg_DE_flush(reg_DE_flush), .forward_ctrl_ls(forward_ctrl_ls),
        .forward_ctrl_A(forward_ctrl_A), .forward_ctrl_B(forward_ctrl_B)
    );

    // EX
    REG_ID_EX reg_ID_EX(
        .clk(debug_clk), .rst(rst), .EN(1'b1), .flush(reg_DE_flush | reg_DE_flush_exp | isFlushed_ID),
        .IR_ID(inst_ID), .PCurrent_ID(PC_ID), .rs1_addr(inst_ID[19:15]), .rs2_addr(inst_ID[24:20]),
        .rs1_data(rs1_data_ID), .rs2_data(rs2_data_ID), .Imm32(Imm_out_ID), .rd_addr(inst_ID[11:7]),
        .ALUSrc_A(ALUSrc_A_ctrl), .ALUSrc_B(ALUSrc_B_ctrl), .ALUC(ALUControl_ctrl),
        .DatatoReg(DatatoReg_ctrl), .RegWrite(RegWrite_ctrl), .WR(mem_w_ctrl),
        .u_b_h_w(inst_ID[14:12]), .mem_r(mem_r_ctrl), .csr_rw(csr_rw_ctrl),
        .csr_w_imm_mux(csr_w_imm_mux_ctrl), .mret(MRET), .exp_vector(exp_vector_ctrl),
        .PCurrent_EX(PC_EXE), .IR_EX(inst_EXE), .rs1_EX(rs1_EXE), .rs2_EX(rs2_EXE),
        .A_EX(rs1_data_EXE), .B_EX(rs2_data_EXE), .Imm32_EX(Imm_EXE), .rd_EX(rd_EXE),
        .ALUSrc_A_EX(ALUSrc_A_EXE), .ALUSrc_B_EX(ALUSrc_B_EXE), .ALUC_EX(ALUControl_EXE),
        .DatatoReg_EX(DatatoReg_EXE), .RegWrite_EX(RegWrite_EXE), .WR_EX(mem_w_EXE),
        .u_b_h_w_EX(u_b_h_w_EXE), .mem_r_EX(mem_r_EXE), .isFlushed(isFlushed_EXE),
        .csr_rw_EX(csr_rw_EXE), .csr_w_imm_mux_EX(csr_w_imm_mux_EXE), .mret_EX(mret_EXE),
        .exp_vector_EX(exp_vector_EXE)
    );

    MUX2T1_32 mux_A_EXE(.I0(rs1_data_EXE), .I1(PC_EXE), .s(ALUSrc_A_EXE), .o(ALUA_EXE));
    MUX2T1_32 mux_B_EXE(.I0(rs2_data_EXE), .I1(Imm_EXE), .s(ALUSrc_B_EXE), .o(ALUB_EXE));
    ALU alu(.A(ALUA_EXE), .B(ALUB_EXE), .Control(ALUControl_EXE), .res(ALUout_EXE), .zero(ALUzero_EXE), .overflow(ALUoverflow_EXE));
    MUX2T1_32 mux_forward_EXE(.I0(rs2_data_EXE), .I1(Datain_MEM), .s(forward_ctrl_ls), .o(Dataout_EXE));

    // MEM
    REG_EX_MEM reg_EXE_MEM(
        .clk(debug_clk), .rst(rst), .EN(1'b1), .flush(reg_EM_flush_exp | isFlushed_EXE),
        .IR_EX(inst_EXE), .PCurrent_EX(PC_EXE), .ALUO_EX(ALUout_EXE), .B_EX(Dataout_EXE),
        .rd_EX(rd_EXE), .rs1_EX(rs1_EXE), .rs1_data_EX(rs1_data_EXE), .DatatoReg_EX(DatatoReg_EXE),
        .RegWrite_EX(RegWrite_EXE), .WR_EX(mem_w_EXE), .u_b_h_w_EX(u_b_h_w_EXE), .mem_r_EX(mem_r_EXE),
        .csr_rw_EX(csr_rw_EXE), .csr_w_imm_mux_EX(csr_w_imm_mux_EXE), .mret_EX(mret_EXE),
        .exp_vector_EX(exp_vector_EXE),
        .PCurrent_MEM(PC_MEM), .IR_MEM(inst_MEM), .ALUO_MEM(ALUout_MEM), .Datao_MEM(Dataout_MEM),
        .rd_MEM(rd_MEM), .rs1_MEM(rs1_MEM), .rs1_data_MEM(rs1_data_MEM), .DatatoReg_MEM(DatatoReg_MEM),
        .RegWrite_MEM(RegWrite_MEM), .WR_MEM(mem_w_MEM), .u_b_h_w_MEM(u_b_h_w_MEM), .mem_r_MEM(mem_r_MEM),
        .isFlushed(isFlushed_MEM), .csr_rw_MEM(csr_rw_MEM), .csr_w_imm_mux_MEM(csr_w_imm_mux_MEM),
        .mret_MEM(mret_MEM), .exp_vector_MEM(exp_vector_MEM)
    );

    RAM_B data_ram(
        .addra(ALUout_MEM), .clka(debug_clk), .dina(Dataout_MEM), .wea(mem_w_MEM), .rea(mem_r_MEM),
        .douta(RAMout_MEM), .mem_u_b_h_w(u_b_h_w_MEM),
        .l_access_fault(l_access_fault_MEM), .s_access_fault(s_access_fault_MEM)
    );

    wire [31:0] csr_w_data_reg_MEM;
    assign csr_w_data_reg_MEM =
                            (RegWrite_WB && (rd_WB != 5'd0) && (rd_WB == rs1_MEM)) ? wt_data_WB :
                            rs1_data_MEM;

    ExceptionUnit exp_unit(.clk(debug_clk),.rst(rst),.csr_rw_in(csr_rw_MEM),.csr_wsc_mode_in(inst_MEM[13:12]),
        .csr_w_imm_mux(csr_w_imm_mux_MEM),.csr_rw_addr_in(inst_MEM[31:20]),
        .csr_w_data_reg(csr_w_data_reg_MEM),.csr_w_data_imm(inst_MEM[19:15]),
        .csr_r_data_out(CSRout_MEM),
        .interrupt(interrupt_pulse),
        .illegal_inst(~isFlushed_WB & exp_vector_WB[3]),
        .ecall_m(~isFlushed_WB & exp_vector_WB[2]),
        .l_access_fault(~isFlushed_WB & exp_vector_WB[1]),
        .s_access_fault(~isFlushed_WB & exp_vector_WB[0]),
        .mret(mret_MEM), .epc_cur(PC_WB),
        .epc_next(~isFlushed_MEM ? PC_MEM : (~isFlushed_EXE ? PC_EXE : (~isFlushed_ID ? PC_ID : PC_IF))),
        .PC_redirect(PC_redirect_exp), .redirect_mux(redirect_mux_exp),
        .reg_FD_flush(reg_FD_flush_exp), .reg_DE_flush(reg_DE_flush_exp),
        .reg_EM_flush(reg_EM_flush_exp), .reg_MW_flush(reg_MW_flush_exp),
        .RegWrite_cancel(RegWrite_cancel_exp)
    );

    MUX2T1_32 mux_csrout(.I0(RAMout_MEM), .I1(CSRout_MEM), .s(csr_rw_MEM), .o(Datain_MEM));

    // WB
    REG_MEM_WB reg_MEM_WB(
        .clk(debug_clk), .rst(rst), .EN(1'b1), .flush(reg_MW_flush_exp | isFlushed_MEM),
        .IR_MEM(inst_MEM), .PCurrent_MEM(PC_MEM), .ALUO_MEM(ALUout_MEM), .Datai(Datain_MEM),
        .rd_MEM(rd_MEM), .DatatoReg_MEM(DatatoReg_MEM), .RegWrite_MEM(RegWrite_MEM),
        .exp_vector_MEM({exp_vector_MEM, l_access_fault_MEM, s_access_fault_MEM}),
        .PCurrent_WB(PC_WB), .IR_WB(inst_WB), .ALUO_WB(ALUout_WB), .MDR_WB(Datain_WB),
        .rd_WB(rd_WB), .DatatoReg_WB(DatatoReg_WB), .RegWrite_WB(RegWrite_WB),
        .isFlushed(isFlushed_WB), .exp_vector_WB(exp_vector_WB)
    );

    MUX2T1_32 mux_WB(.I0(ALUout_WB), .I1(Datain_WB), .s(DatatoReg_WB), .o(wt_data_WB));

    wire [31:0] Test_signal;
    assign debug_data = debug_addr[5] ? Test_signal : Debug_regs;

    CPUTEST U1_3(
        .PC_IF(PC_IF), .PC_ID(PC_ID), .PC_EXE(PC_EXE), .PC_MEM(PC_MEM), .PC_WB(PC_WB),
        .PC_next_IF(next_PC_IF), .PCJump(jump_PC_ID),
        .inst_IF(inst_IF), .inst_ID(inst_ID), .inst_EXE(inst_EXE), .inst_MEM(inst_MEM), .inst_WB(inst_WB),
        .PCEN(PC_EN_IF), .Branch(Branch_ctrl), .PCSource(Branch_ctrl),
        .RS1DATA(rs1_data_reg), .RS2DATA(rs2_data_reg), .Imm32(Imm_out_ID), .ImmSel(ImmSel_ctrl),
        .ALUC(ALUControl_ctrl), .ALUSrc_A(ALUSrc_A_ctrl), .ALUSrc_B(ALUSrc_B_ctrl),
        .A(ALUA_EXE), .B(ALUB_EXE), .ALU_out(ALUout_MEM), .Datai(Datain_MEM), .Datao(Dataout_MEM),
        .Addr(Addr), .WR(MWR), .MIO(mem_r_MEM), .WDATA(wt_data_WB), .DatatoReg(DatatoReg_WB), .RegWrite(RegWrite_WB),
        .data_hazard(reg_FD_stall), .control_hazard(Branch_ctrl),
        .exp_sig({csr_rw_MEM,2'b0,csr_w_imm_mux_MEM,
                  isFlushed_ID,isFlushed_EXE,isFlushed_MEM,isFlushed_WB,
                  interrupt_pulse,3'b0,exp_vector_WB,
                  4'b0,
                  3'b0,redirect_mux_exp,
                  reg_FD_flush_exp,reg_DE_flush_exp,reg_EM_flush_exp,reg_MW_flush_exp,
                  3'b0,RegWrite_cancel_exp}),
        .Debug_addr(debug_addr[4:0]), .Test_signal(Test_signal)
    );

endmodule

```