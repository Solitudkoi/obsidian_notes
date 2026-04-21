# Lab 2: Pipelined CPU Supporting Exception & Interrupt

This experiment extends the Lab 1 five-stage pipelined RISC-V RV32I CPU with Machine-mode exception and interrupt support.

---

## 📁 Project Structure

```
lab2/
├── code/          Verilog source, testbench, hex files, and constraints
├── figures/       Simulation waveform screenshots
├── report.pdf     Lab report
└── README.md      This file
```

---

## ✅ Completed Tasks

### 1. ExceptionUnit (`core/ExceptionUnit.v`)
The core module designed in this experiment. It implements:
- **Four CSR registers**: mstatus (0x300), mtvec (0x305), mepc (0x341), mcause (0x342)
- **Three-state FSM** for trap entry: IDLE → MEPC → MCAUSE → IDLE, serializing three CSR writes across three cycles due to the single-write-port constraint
- **Six CSR instructions**: csrrw/csrrs/csrrc and csrrwi/csrrsi/csrrci, via `csr_wsc_mode_in` (write/set/clear) and `csr_w_imm_mux` (rs1/zimm source selection)
- **Precise exception**: all exceptions commit at WB stage; FD/DE/EM flush + RegWrite_cancel ensure faulting and subsequent instructions do not modify machine state
- **mret handling**: restores mstatus and redirects PC to mepc in a single cycle (no FSM needed)

### 2. Datapath Modifications (`core/RV32core.v`)
- PC redirection mux controlled by ExceptionUnit
- Exception vector (`exp_vector`) propagation through pipeline registers
- `isFlushed` flag to prevent flushed bubbles from re-triggering traps
- CSR read data merged into the writeback path via mux

### 3. Controller Updates (`core/CtrlUnit.v`)
- Decoding of ecall, mret, and CSR instructions
- Generation of `csr_rw`, `csr_wsc_mode`, `csr_w_imm_mux`, and `mret` control signals

---

## 🔧 Supported Trap Types

| Trap | Cause | Detection |
|------|-------|-----------|
| Illegal instruction | 2 | ID stage |
| Load access fault | 5 | MEM stage |
| Store access fault | 7 | MEM stage |
| ecall (M-mode) | 11 | ID stage |
| External interrupt (SW[12]) | 0x8000000B | External |

---

## 🔬 Verification

**CP1 (Simulation)**: The verification program triggers ecall → illegal → load fault → store fault in sequence, each handled by a shared trap handler at 0x78 that reads CSRs, advances mepc by 4, and returns via mret. Waveforms in `figures/`.

**CP2 (FPGA)**: SW[12] triggers an external interrupt; Code2Inst displays CSR instruction decoding on the seven-segment display.

---

## 📄 Report

See `report.pdf` for detailed design analysis, code explanation, and waveform interpretation.