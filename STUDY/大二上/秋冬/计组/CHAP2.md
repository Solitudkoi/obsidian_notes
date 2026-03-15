![[Pasted image 20251029193557.png]]
![[Pasted image 20251029203755.png]]
![[Pasted image 20251029203811.png]]
![[Pasted image 20251029205244.png]]![[Pasted image 20251029205303.png]]
![[Pasted image 20251029205745.png]]
![[Pasted image 20251030082052.png]]![[Pasted image 20251109111748.png]]
```
jal x1, fact

fact: addi sp, sp, -16
      sd x1, 8(sp)
      sd x10, 0(sp)
      beq x10, x0, L1
      addi x5, x0, 1
	  beq x10, x5, L2
      addi x10, x10, -1
      jal x1, fact
      add x9, x10, x0
      ld x10, 0(sp)
      addi x10, x10, -2
      jal x1, fact
      add x10, x10, x9
      ld x1, 8(sp)
      addi sp, sp, 16
      jalr x0, 0(x1)

L1:   addi x10, x0, 0
      addi sp, sp, 16
      jalr x0, 0(x1)

L2:   addi x10, x0, 1
      addi sp, sp, 16
      jalr x0, 0(x1)
```