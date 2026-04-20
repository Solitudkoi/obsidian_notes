# Pipelined CPU Supporting Exception & Interrupt

This project extends the Lab 1 five-stage pipelined RISC-V RV32I CPU with Machine-mode exception and interrupt support, following the RISC-V privileged architecture specification.

---

## 📁 Project Structure

```
lab2/
├── code/
│   ├── core/                   Core CPU modules
│   │   ├── ExceptionUnit.v     ★ Exception/interrupt handling unit (key module)
│   │   ├── CSRRegs.v           CSR register file
│   │   ├── RV32core.v          Top-level CPU datapath and pipeline integration
│   │   ├── CtrlUnit.v          Control unit with CSR/ecall/mret decoding
│   │   ├── HazardDetectionUnit.v  Hazard detection and forwarding
│   │   ├── ALU.v               Arithmetic logic unit
│   │   ├── ImmGen.v            Immediate generator
│   │   ├── Regs.v              Register file (x0-x31)
│   │   ├── ROM_D.v             Instruction memory (loads rom.hex)
│   │   ├── RAM_B.v             Data memory with access-fault detection
│   │   ├── REG_IF_ID.v         IF/ID pipeline register
│   │   ├── REG_ID_EX.v         ID/EX pipeline register
│   │   ├── REG_EX_MEM.v        EX/MEM pipeline register
│   │   ├── REG_MEM_WB.v        MEM/WB pipeline register
│   │   ├── rom.hex             Instruction memory initialization
│   │   └── ram.hex             Data memory initialization
│   ├── common/                 Shared utility modules
│   │   ├── MUX2T1_32.v         2-to-1 multiplexer
│   │   ├── MUX4T1_32.v         4-to-1 multiplexer
│   │   ├── REG32.v             32-bit register
│   │   ├── add_32.v            32-bit adder
│   │   └── cmp_32.v            32-bit comparator
│   ├── auxillary/              Board-level support modules
│   │   ├── top.v               FPGA top-level module
│   │   ├── Code2Inst.v         Instruction disassembler for display
│   │   ├── CPUTEST.v           Debug signal multiplexer
│   │   ├── debug_clk.v         Debug clock generator
│   │   └── ...                 VGA, display, button scan, etc.
│   ├── sim/                    Simulation files
│   │   ├── core_sim.v          Testbench with interrupt injection
│   │   ├── core_sim_behav.wcfg Vivado waveform configuration
│   │   └── example.png         Reference waveform
│   ├── constraint.xdc          FPGA pin constraints
│   └── exp_test.s              Verification program (assembly)
├── figures/                    Simulation waveform screenshots (1-7.png)
├── report.pdf                  Lab report
└── README.md                   This file
```

---

## 🧩 Core Design: ExceptionUnit

The `ExceptionUnit` is the central module added in this experiment. It implements:

### CSR Registers
Four Machine-mode CSRs: `mstatus` (0x300), `mtvec` (0x305), `mepc` (0x341), `mcause` (0x342).

### Three-State FSM for Trap Entry
Since the CSR register file has only one write port, the three CSRs (mstatus, mepc, mcause) are updated across three cycles:

| State | Action |
|-------|--------|
| IDLE → MEPC | Write mstatus (MPIE←MIE, MIE←0); latch epc/cause; flush pipeline |
| MEPC → MCAUSE | Write mepc from latched epc_reg |
| MCAUSE → IDLE | Write mcause from latched cause_reg |

### CSR Instructions
All six CSR instructions are supported through two control signals:
- `csr_w_imm_mux`: selects between rs1 (register) and zimm (immediate) source
- `csr_wsc_mode_in`: selects write (01), set (10), or clear (11) operation

### Precise Exception Semantics
- All exceptions are committed at the **WB stage** to ensure preceding instructions complete
- `RegWrite_cancel` prevents the faulting instruction from modifying the register file
- Pipeline flush (FD/DE/EM) clears in-flight instructions after the fault
- MW register is never flushed (earlier instructions must commit normally)

---

## 🔧 Supported Trap Types

| Trap | Cause Code | Detection Stage |
|------|-----------|-----------------|
| Illegal instruction | 2 | ID (decode) |
| Load access fault | 5 | MEM (RAM_B) |
| Store access fault | 7 | MEM (RAM_B) |
| Environment call (ecall) | 11 | ID (decode) |
| Machine external interrupt | 0x8000000B | External (SW[12]) |

---

## 🔬 Verification

### Simulation (CP1)
The verification program (`rom.hex`) sequentially triggers all four synchronous exceptions:
1. `ecall` at PC=0x38 → handler → mret back to 0x3C
2. Illegal instruction at PC=0x40 → handler → mret back to 0x44
3. Load access fault at PC=0x4C → handler → mret back to 0x50
4. Store access fault at PC=0x54 → handler → mret back to 0x58

Waveform screenshots are in `figures/1.png` through `figures/7.png`.

### FPGA (CP2)
- **SW[12] interrupt**: toggleing SW[12] triggers a machine external interrupt with cause=0x8000000B
- **Code2Inst display**: all six CSR instructions are correctly decoded and displayed on the seven-segment display

---

## 🚀 How to Run

### Simulation
1. Open the Vivado project
2. Ensure `rom.hex` and `ram.hex` are in the xsim working directory (`Exp2.sim/sim_1/behav/xsim/`)
3. Run Behavioral Simulation with `core_sim` as the top module

### FPGA
1. Synthesize and implement the design with `top.v` as the top module
2. Generate bitstream and program the FPGA board
3. Use single-step clock to observe each instruction
4. Toggle SW[12] to trigger an external interrupt

---

## 📄 Report

The detailed design analysis and waveform explanations are documented in `report.pdf`.
