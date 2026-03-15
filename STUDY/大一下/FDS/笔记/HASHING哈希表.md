for searching 高效查找
不做比较（突破下限）
* Interpolation Search
	![[Pasted image 20250524221150.png]]
	best/average: log(log N)
	worst: N
	search by formula 映射
* A. General Idea
	![[Pasted image 20250524221604.png]]
	![[Pasted image 20250524222056.png]]
	collision（有余量）冲突->往后放
	overflow->要扩容
	![[Pasted image 20250524222526.png]]
	哈希表实现效率：哈希设计实现的效率+哈希表结构
* B. Hash Function
	![[Pasted image 20250524223023.png]]
	字符串![[Pasted image 20250525161014.png]]
	ASCII求和取余  要考虑字符出现顺序  27进制（为什么是27？26+1？）
	![[Pasted image 20250525161559.png]]
	32进制的十进制转换
	高度定制化
* C. Separate Chaining   分离链表 额外空间损耗 效率变低
	bucket用链表
	![[Pasted image 20250525162617.png]]
	![[Pasted image 20250525162649.png]]
	dummy head  
	![[Pasted image 20250525163159.png]]
	效率   常数
 	![[Pasted image 20250525171416.png]]
* D. Open Addressing  开放寻址  空位找不到
	偏移量 
	![[Pasted image 20250525172212.png]]
	![[Pasted image 20250525172318.png]]
	![[Pasted image 20250525172832.png]]
	缺陷：滚雪球，区间会变大，search time会变大
	![[Pasted image 20250525173102.png]]
	有空格，
	![[Pasted image 20250528144944.png]]
	![[Pasted image 20250528144959.png]]
	![[Pasted image 20250528145022.png]]
* F. Rehashing
	扩容开新表（至少两倍大prime），扫描原表
	need a new hashing function
	












































#### 7-1 Hashing - Hard Version
给定一个大小为N的哈希表，我们可以定义一个哈希函数 。假设使用线性探测法解决冲突，给定一个输入数字序列，我们可以很容易地得到哈希表的状态。
然而，现在要求你解决反向问题：根据给定的哈希表状态重建输入序列。每当有多种选择时，总是选择最小的数字。 
输入规范：
每个输入文件包含一个测试用例。对于每个测试用例，第一行包含一个正整数（），它是哈希表的大小。下一行包含 个整数，用空格分隔。负整数表示哈希表中的空单元格。保证表中所有非负整数都是不同的。
输出规范：
对于每个测试用例，打印一行，其中包含输入序列，数字之间用空格分隔。请注意，每行末尾不能有多余的空格。