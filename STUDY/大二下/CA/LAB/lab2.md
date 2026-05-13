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

    // CSR addresses
    localparam CSR_MSTATUS = 12'h300;
    localparam CSR_MTVEC   = 12'h305;
    localparam CSR_MEPC    = 12'h341;
    localparam CSR_MCAUSE  = 12'h342;

    // mstatus bits used in this lab
    // MIE  = bit[3]
    // MPIE = bit[7]


    wire sync_exception;
    assign sync_exception = illegal_inst | ecall_m | l_access_fault | s_access_fault;

    // CSR read mux
    always @(*) begin
        case (csr_rw_addr_in)
            CSR_MSTATUS: csr_r_data_out = mstatus;
            CSR_MTVEC  : csr_r_data_out = mtvec;
            CSR_MEPC   : csr_r_data_out = mepc;
            CSR_MCAUSE : csr_r_data_out = mcause;
            default    : csr_r_data_out = 32'h0;
        endcase
    end



    wire trap_req_wb;
    assign trap_req_wb = interrupt | illegal_inst | ecall_m | l_access_fault | s_access_fault;

    always @(*) begin
        redirect_mux = 1'b0;
        PC_redirect  = 32'h00000000;

        if (trap_req_wb) begin
            redirect_mux = 1'b1;
            PC_redirect  = {mtvec[31:2], 2'b00};   // direct mode
        end
        else if (mret) begin
            redirect_mux = 1'b1;
            PC_redirect  = mepc;
        end
    end

    // flush strategy:
    // trap / mret happens after younger insts have entered pipeline, so flush all front stages.
    assign reg_FD_flush = redirect_mux;
    assign reg_DE_flush = redirect_mux;
    assign reg_EM_flush = trap_req_wb;   // trap kills younger instructions
    assign reg_MW_flush = 1'b0;       // current WB is trap source itself, usually not needed to flush MW

    // cancel WB only for synchronous exception, not interrupt, not mret
    assign RegWrite_cancel = sync_exception;

    // helpers
    reg [31:0] csr_old;
    reg [31:0] csr_wdata;
    reg [31:0] csr_src;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            mstatus <= 32'h0000_0008; // MIE=1 can also be 0 depending on teacher design; 8 is common lab default
            mtvec   <= 32'h0000_0000;
            mepc    <= 32'h0000_0000;
            mcause  <= 32'h0000_0000;
        end
        else begin
            // ----------------------------
            // Priority 1: trap entry
            // ----------------------------
            if (trap_req_wb) begin
                // save exception pc:
                // interrupt -> next PC
                // synchronous exception -> current PC
                if (interrupt)
                    mepc <= epc_next;
                else
                    mepc <= epc_cur;

                // mcause
                if (interrupt)
                    mcause <= 32'h8000_000b;  // machine external interrupt (common lab choice)
                else if (illegal_inst)
                    mcause <= 32'd2;
                else if (ecall_m)
                    mcause <= 32'd11;
                else if (l_access_fault)
                    mcause <= 32'd5;
                else if (s_access_fault)
                    mcause <= 32'd7;

                // mstatus: MPIE <= MIE, MIE <= 0
                mstatus[7] <= mstatus[3];
                mstatus[3] <= 1'b0;
            end

            // ----------------------------
            // Priority 2: mret
            // ----------------------------
            else if (mret) begin
                // mstatus: MIE <= MPIE, MPIE <= 1
                mstatus[3] <= mstatus[7];
                mstatus[7] <= 1'b1;
            end

            // ----------------------------
            // Priority 3: normal CSR write in MEM
            // ----------------------------
            else if (csr_rw_in) begin
                csr_src = csr_w_imm_mux ? {27'b0, csr_w_data_imm} : csr_w_data_reg;

                case (csr_rw_addr_in)
                    CSR_MSTATUS: begin
                        csr_old = mstatus;
                        case (csr_wsc_mode_in)
                            2'b01: csr_wdata = csr_src;           // csrrw / csrrwi
                            2'b10: csr_wdata = csr_old | csr_src; // csrrs / csrrsi
                            2'b11: csr_wdata = csr_old & ~csr_src;// csrrc / csrrci
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
                        // unsupported CSR: do nothing
                    end
                endcase
            end
        end
    end

endmodule
```