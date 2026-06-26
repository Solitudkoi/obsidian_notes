# Computer Architecture — Assignment 3
## English Answers for Q3, Q12, Q22, Q26, Q31 (the five "exam big-problem" questions)

> Worked examples are aligned to this course's actual exam parameters (from your lecture
> slides / past papers), so each answer doubles as 大题 practice.

---

## Q3. Amdahl's Law — conclusion and speedup computation

**Conclusion.** The overall speedup obtained by improving one part of a system is limited bythe fraction of time that part is actually used. No matter how fast you make that part, the*unimproved* portion bounds the total gain. (This is the quantitative basis for "make thecommon case fast.")

**Single enhanced portion:**
$$\text{Speedup} = \frac{1}{(1-f) + \dfrac{f}{s}}, \qquad \lim_{s\to\infty}\text{Speedup} = \frac{1}{1-f}$$
where $f$ = fraction of original execution time that is enhanced, $s$ = speedup of that portion.

**Multiple enhanced portions:**
$$\text{Speedup} = \frac{1}{\Big(1-\sum_i f_i\Big) + \sum_i \dfrac{f_i}{s_i}}$$

**Example 1 (single portion).** 60% of a program is parallelizable ($f=0.6$) and that part runs
2× faster ($s=2$):
$$\text{Speedup} = \frac{1}{0.4 + 0.6/2} = \frac{1}{0.7} \approx 1.43$$
Even with $s\to\infty$ the ceiling is only $1/0.4 = 2.5$.

**Example 2 (two portions).** 30% sped up 2×, 20% sped up 5×, remaining 50% unchanged:
$$\text{Speedup} = \frac{1}{0.5 + \dfrac{0.3}{2} + \dfrac{0.2}{5}} = \frac{1}{0.69} \approx 1.45$$

**Example 3 (reverse-solve — common exam variant).** *"What fraction of a program must beparallelizable to reach an overall speedup of 5 on a machine with infinitely many processors?"* With $s\to\infty$, $\text{Speedup}=1/(1-f)$, so
$$\frac{1}{1-f} \ge 5 \;\Rightarrow\; f \ge 0.8 \;(80\%).$$
With a **finite** $n$ processors, set the target $S$ and solve for $f$:
$$S = \frac{1}{(1-f) + f/n} \;\Rightarrow\; f = \frac{1 - 1/S}{1 - 1/n}.$$
E.g. target $S=4$ on $n=8$ cores: $f = (1-0.25)/(1-0.125) = 0.75/0.875 \approx 0.857$.

**Takeaway:** always attack the portion with the **largest $f$**; the serial remainder sets the
ceiling — this is also why multicore scaling saturates.

---

## Q12. Cache address mapping + write-back / write-allocate access trace

**Address split (byte-addressed):**
$$\underbrace{\text{Tag}}\ \big|\ \underbrace{\text{Index}}\ \big|\ \underbrace{\text{Block offset}}$$
- offset bits $= \log_2(\text{block size})$
- index bits $= \log_2(\#\text{sets})$, where $\#\text{sets} = \dfrac{\text{cache size}}{\text{block size}\times\text{associativity}}$
- tag bits $= \text{address width} - \text{index} - \text{offset}$

**Course exam configuration (32-bit addresses):**

| | Size | Block | Assoc. | Offset | #sets | Index | Tag |
|---|---|---|---|---|---|---|---|
| **L1** | 16 KB | 1 KB | direct-mapped | 10 | 16 | 4 | 18 |
| **L2** | 1 MB | 1 KB | 4-way | 10 | 256 | 8 | 14 |

*L1:* #blocks $=16\text{KB}/1\text{KB}=16$, direct-mapped ⇒ 16 sets ⇒ index $=\log_2 16=4$, tag $=32-4-10=18$.
*L2:* #blocks $=1\text{MB}/1\text{KB}=1024$, 4-way ⇒ 256 sets ⇒ index $=\log_2 256=8$, tag $=32-8-10=14$.

**Decode the two addresses** (offset = low 10 bits = `0x400 & 0x3FF = 0` for both):

| Address | L1 index | L1 tag | L2 index | L2 tag |
|---|---|---|---|---|
| `0x0000_0400` | 1 | 0 | 1 | 0 |
| `0x0010_0400` | 1 | 64 (0x40) | 1 | 4 |

> Both map to **the same set/index at every level** (index 1) but carry **different tags**.
> In the **direct-mapped L1** they therefore **conflict** (same line, different tag).

**Access trace — write `0x00000400`, then read `0x00100400`** (write-back + write-allocate):

*Step 1 — Store to `0x00000400`*
1. L1[idx 1]: invalid (cold) → **L1 miss**.
2. Write-allocate ⇒ fetch the block. Check L2[idx 1, tag 0]: cold → **L2 miss** → fetch from memory into L2, then into L1.
3. Perform the write in L1. L1[idx 1] now: `valid=1, tag=0, dirty=1`.

*Step 2 — Load from `0x00100400`*
1. L1[idx 1] holds `tag=0` but request needs `tag=64` → **tag mismatch ⇒ L1 conflict miss**.
2. Victim block is **dirty** ⇒ **write back** the old block to L2 (set 1, tag 0).
3. Allocate the requested block: L2[idx 1] is 4-way and has room; `tag=4` is cold → **L2 miss** → fetch from memory → fill L2 and L1.
4. Load completes. L1[idx 1] now: `valid=1, tag=64, dirty=0` (clean, it was a read).

**State of L1 line (index 1) across the trace:**

| After | valid | tag | dirty |
|---|---|---|---|
| (init) | 0 | – | – |
| Store `0x00000400` | 1 | 0 | 1 |
| Load `0x00100400` | 1 | 64 | 0 |

Key exam points: (a) the two addresses **collide in the direct-mapped L1**; (b) the dirty victim
must be **written back** (write-back) before the new block loads; (c) with **write-allocate**, even
a *store* miss first fetches the block.

---

## Q22. Dynamic-scheduling timeline (Tomasulo) — course's own instruction sequence

**Instruction sequence:**
```
1:  div x2, x3, x4
2:  mul x1, x5, x6
3:  add x3, x7, x8
4:  mul x1, x1, x3      ; needs x1 (from #2), x3 (from #3)
5:  sub x4, x1, x5      ; needs x1 (from #4)
6:  sub x1, x4, x2      ; needs x4 (from #5), x2 (from #1)
```

**Assumptions (state these on the exam):**
- Execution latencies: **div = 40, mul = 10, add/sub = 1** cycles.
- **Single issue**, in-order, 1 instruction per cycle.
- **One reservation station per FU type** (so a second mul must wait for the first mul's RS).
- A reservation station is **freed when its result is broadcast** on the CDB.
- Timing model: **Issue** takes 1 cycle; **Execute starts the cycle after issue** if operands are
  ready, otherwise the cycle after the producing instruction broadcasts; **Write/CDB** occurs the
  cycle after execution completes.

**Tomasulo cycle table (official course numbers):**

| # | Instruction | Issue | Exec (start–end) | Write CDB | Reason for any stall |
|---|---|---|---|---|---|
| 1 | div x2,x3,x4 | 1 | 2 – 41 | **42** | operands ready; 40-cycle div |
| 2 | mul x1,x5,x6 | 2 | 3 – 12 | 13 | operands ready; 10-cycle mul |
| 3 | add x3,x7,x8 | 3 | 4 – 4 | 5 | operands ready; 1-cycle add |
| 4 | mul x1,x1,x3 | **14** | 15 – 24 | 25 | **structural**: only 1 mul RS, freed when #2 broadcasts (13) ⇒ issue at 14. x1(13), x3(5) already ready |
| 5 | sub x4,x1,x5 | 15 | 26 – 26 | 27 | **RAW** on x1 from #4 (CDB at 25) ⇒ exec at 26 |
| 6 | sub x1,x4,x2 | 16 | **43** – 43 | 44 | **RAW** on x2 from the 40-cycle div (#1, CDB at 42) ⇒ exec at 43 |

**The three "why can't it go one cycle earlier?" points (highest-yield on the exam):**
- **#4 issues at 14, not 4** — *structural hazard*: the single mul reservation station is occupied by
  #2 until #2 broadcasts at cycle 13.
- **#6 executes at 43, not earlier** — *data (RAW) hazard*: it needs `x2`, produced by the 40-cycle
  `div`, which broadcasts at 42; the dependent starts one cycle later (43).
- **#5 vs #6** — #5 only waits on `x1` (ready at 25), while #6 waits on the much-later `x2` (42),
  which is why #6 is the last to execute even though it issued just after #5.

**Scoreboard contrast (rules + grading focus).** Four stages: **Issue / Read-operands / Execute /
Write-result**. Where each hazard stalls:

| Hazard | Stalls at |
|---|---|
| Structural & **WAW** | Issue |
| **RAW** | Read-operands |
| **WAR** | Write-result |

Scoreboarding has **no register renaming and no forwarding** (operands are read from the register
file *after* the producer writes back). The exact cycle numbers shift by ±1 with convention — the
graded point is **which hazard stalls which stage**, which is fixed.

**Renaming benefit (Tomasulo vs scoreboard).** Tomasulo uses RS tags to rename registers, so
**WAR and WAW never stall**. In this sequence both `mul`s and the last `sub`s reuse `x1`
(output dependence / WAW) and reuse `x4`; under scoreboarding these would add Issue/Write stalls,
whereas Tomasulo resolves them through tags on the CDB.

---

## Q26. Vector execution time and why chaining helps

**Key terms:**
- **Convoy** — a set of vector instructions that *could begin execution together* because they have
  no structural hazard and no data hazard among them.
- **Chime** — the time to execute one convoy, ≈ **vector length $n$ cycles** (ignoring startup
  overhead). Counting chimes gives a quick performance estimate.

**Execution time.** For $m$ convoys, vector length $n$, and per-convoy startup overhead $T_{start}$:
$$T_{total} \;\approx\; \sum_{i=1}^{m}\big(T_{start,i} + n\big) \;\approx\; m\cdot n \ \text{chimes (overhead ignored)}.$$

**It depends on:**
1. **Vector length $n$**;
2. **Number of convoys $m$** — determined by structural hazards and data dependences;
3. **Startup / pipeline latency** of the functional units and memory;
4. **Number of lanes** — with $L$ lanes each convoy takes ≈ $n/L$ cycles.

**Why chaining optimizes execution time.**
Without chaining, two vector instructions with a RAW dependence must go in **separate convoys**:
the first must write its *entire* result vector before the second can start. Chaining lets the
dependent instruction **start consuming results element-by-element as soon as the first element of
the source vector is produced** (element-level forwarding). The dependent op can then sit in the
**same convoy** ⇒ fewer convoys/chimes ⇒ shorter total time. Intuitively, chaining links several
vector functional units into one long pipeline.

**Worked example (Cray-1 style).** `MULV V1,V2,V3` then `ADDV V4,V1,V5` (RAW on V1), length $n$:

| | Convoys | Time (schematic) |
|---|---|---|
| **No chaining** | 2 | $(T_{start,mul} + n) + (T_{start,add} + n) \approx 2n + \text{overhead}$ |
| **With chaining** | 1 | $T_{start,mul} + T_{start,add} + n \approx n + \text{overhead}$ |

In this course's three-instruction (load + mul + add) example the cost collapses roughly from
**$\sim 3N + 22$ (unchained)** toward **$\sim N + 16$ (fully chained)** — i.e. chaining turns
several chimes into essentially one pass plus the combined startup latencies. Adding more **lanes**
shortens it further to about $n/L$ per convoy.

---

## Q31. Cache coherence — snooping vs directory (MSI), worked example

**Setup.** 3 cores P0 / P1 / P2, each with a private write-back cache; one block **A**; protocol
**MSI** (Modified / Shared / Invalid); all caches start **I**, memory holds A.

**Access sequence:** (1) P0 reads A, (2) P1 reads A, (3) P0 writes A, (4) P2 reads A.

### (a) Snooping (invalidation-based, shared bus)

| Step | Action | Bus transaction | P0 | P1 | P2 |
|---|---|---|---|---|---|
| 0 | initial | — | I | I | I |
| 1 | P0 read (miss) | **BusRd**, memory supplies | **S** | I | I |
| 2 | P1 read (miss) | **BusRd**, memory/P0 supplies | S | **S** | I |
| 3 | P0 write | **BusRdX / BusUpgr**, invalidates others | **M** | **I** | I |
| 4 | P2 read (miss) | **BusRd**; P0(M) supplies & writes back, downgrades | **S** | I | **S** |

Mechanics: a write **broadcasts an invalidate**; when another cache holds the block in **M**, that
cache supplies the data (cache-to-cache) and writes back.

### (b) Directory-based (home directory holds {state, sharer bit-vector})

| Step | Action | Message to home | Dir state | Sharers | P0 | P1 | P2 |
|---|---|---|---|---|---|---|---|
| 0 | initial | — | Uncached | {} | I | I | I |
| 1 | P0 read | Read miss | Shared | {P0} | **S** | I | I |
| 2 | P1 read | Read miss | Shared | {P0,P1} | S | **S** | I |
| 3 | P0 write | Write/Upgrade | Modified (owner P0) | {P0} | **M** | **I** (dir sends invalidate to P1) | I |
| 4 | P2 read | Read miss | Shared | {P0,P2} | **S** (dir makes P0 write back) | I | **S** |

Mechanics: the directory uses **point-to-point messages** (scalable, no broadcast); on a write it
**invalidates only the current sharers**; when a requested block is held **M**, the directory
**directs the owner to write back / forward** the data.

**Snooping vs directory.** Snooping relies on **broadcast + bus serialization** — simple but does
not scale; directory relies on a **central/distributed directory + targeted messages** — scalable
but with extra directory storage and protocol complexity.
