![[Pasted image 20250529133854.png]]

* Dedicated MUX-Based Transfers
![[Pasted image 20250529140303.png]]
三个寄存器的值可以同时赋值（轮换）同时进行数据传输

![[Pasted image 20250529140757.png]]
无法同时进行数据传输

三态门使能信号为0->高阻
![[Pasted image 20250529144352.png]]

![[Pasted image 20250529144313.png]]
![[Pasted image 20250529144059.png]]
会有延时，当最高位变化时延时最大
![[Pasted image 20250529144158.png]]

![[Pasted image 20250529144443.png]]
![[Pasted image 20250529144748.png]]
也会有较大延时，时钟频率无法提高



可能因为延时（异步），同时为1，（错误地）清零



![[Pasted image 20250529154624.png]]
