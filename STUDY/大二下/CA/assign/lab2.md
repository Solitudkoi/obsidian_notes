```
`timescale 1ns / 1ps

module ExceptionUnit(
    input clk, rst,
    input csr_rw_in,
    input[1:0] csr_wsc_mode_in,
    input csr_w_imm_mux,
    input[11:0] csr_rw_addr_in,
    input[31:0] csr_w_data_reg,
    input[4:0] csr_w_data_imm,
    output[31:0] csr_r_data_out,

    input interrupt,
    input illegal_inst,
    input l_access_fault,
    input s_access_fault,
    input ecall_m,

    input mret,

    input[31:0] epc_cur,
    input[31:0] epc_next,
    output[31:0] PC_redirect,
    output redirect_mux,

    output reg_FD_flush, reg_DE_flush, reg_EM_flush, reg_MW_flush,
    output RegWrite_cancel
);

    reg[11:0] csr_raddr, csr_waddr;
    reg[31:0] csr_wdata;
    reg csr_w;
    reg[1:0] csr_wsc;

    wire[31:0] mstatus;

    CSRRegs csr(.clk(clk),.rst(rst),.csr_w(csr_w),.raddr(csr_raddr),.waddr(csr_waddr),
        .wdata(csr_wdata),.rdata(csr_r_data_out),.mstatus(mstatus),.csr_wsc_mode(csr_wsc));

    localparam STATE_IDLE   = 2'd0;
    localparam STATE_MEPC   = 2'd1;
    localparam STATE_MCAUSE = 2'd2;

    localparam CSR_MSTATUS = 12'h300;
    localparam CSR_MTVEC   = 12'h305;
    localparam CSR_MEPC    = 12'h341;
    localparam CSR_MCAUSE  = 12'h342;

    localparam CSR_WSC_W = 2'b01;

    reg [1:0] state, next_state;
    reg [31:0] epc_reg, cause_reg;

    wire trap_req = interrupt | illegal_inst | l_access_fault | s_access_fault | ecall_m;
    wire [31:0] trap_cause = interrupt      ? 32'h8000000b :
                             illegal_inst   ? 32'd2        :
                             l_access_fault ? 32'd5        :
                             s_access_fault ? 32'd7        :
                             ecall_m        ? 32'd11       : 32'd0;
    wire [31:0] trap_epc = interrupt ? epc_next : epc_cur;

    wire [31:0] mstatus_enter = {mstatus[31:8], mstatus[3], mstatus[6:4], 1'b0, mstatus[2:0]};
    wire [31:0] mstatus_exit  = {mstatus[31:8], 1'b1,       mstatus[6:4], mstatus[7], mstatus[2:0]};

    always @(posedge clk) begin
        if (rst) begin
            state <= STATE_IDLE;
            epc_reg <= 32'd0;
            cause_reg <= 32'd0;
        end else begin
            state <= next_state;
            if (state == STATE_IDLE && trap_req) begin
                epc_reg <= trap_epc;
                cause_reg <= trap_cause;
            end
        end
    end

    always @(*) begin
        next_state = state;

        csr_raddr = csr_rw_addr_in;
        csr_waddr = csr_rw_addr_in;
        csr_wdata = csr_w_imm_mux ? {27'd0, csr_w_data_imm} : csr_w_data_reg;
        csr_w = 1'b0;
        csr_wsc = csr_wsc_mode_in;

        case (state)
            STATE_IDLE: begin
                if (trap_req) begin
                    next_state = STATE_MEPC;
                    csr_w = 1'b1;
                    csr_waddr = CSR_MSTATUS;
                    csr_wdata = mstatus_enter;
                    csr_wsc = CSR_WSC_W;
                end else if (mret) begin
                    next_state = STATE_IDLE;
                    csr_raddr = CSR_MEPC;
                    csr_w = 1'b1;
                    csr_waddr = CSR_MSTATUS;
                    csr_wdata = mstatus_exit;
                    csr_wsc = CSR_WSC_W;
                end else if (csr_rw_in) begin
                    next_state = STATE_IDLE;
                    csr_raddr = csr_rw_addr_in;
                    csr_w = 1'b1;
                    csr_waddr = csr_rw_addr_in;
                    csr_wdata = csr_w_imm_mux ? {27'd0, csr_w_data_imm} : csr_w_data_reg;
                    csr_wsc = csr_wsc_mode_in;
                end
            end

            STATE_MEPC: begin
                next_state = STATE_MCAUSE;
                csr_raddr = CSR_MTVEC;
                csr_w = 1'b1;
                csr_waddr = CSR_MEPC;
                csr_wdata = epc_reg;
                csr_wsc = CSR_WSC_W;
            end

            STATE_MCAUSE: begin
                next_state = STATE_IDLE;
                csr_w = 1'b1;
                csr_waddr = CSR_MCAUSE;
                csr_wdata = cause_reg;
                csr_wsc = CSR_WSC_W;
            end

            default: begin
                next_state = STATE_IDLE;
            end
        endcase
    end

    assign redirect_mux = (state == STATE_MEPC) | ((state == STATE_IDLE) & mret);
    assign PC_redirect  = ((state == STATE_IDLE) & mret) ? csr_r_data_out :
                          (state == STATE_MEPC)          ? csr_r_data_out : 32'd0;

    assign reg_FD_flush = trap_req | (state == STATE_MEPC) | mret;
    assign reg_DE_flush = trap_req | mret;
    assign reg_EM_flush = trap_req | mret;
    assign reg_MW_flush = trap_req;
    assign RegWrite_cancel = trap_req & ~interrupt;

endmodule

```