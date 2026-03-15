#Quicksort
* A. **Algorithm**

	![[Pasted image 20250514142231.png]]
	partition->递归
	前后组数据个数不同，
	遍历，递归
	best case T(N)=O(N log N)
* B.**Picking the pivot**
	❌  1.A[0] worst case  O(N^2)
		2.random
	✅1.median of three
		Pivot=median (left,center,right)
		2.![[Pasted image 20250514143547.png]]
* C.**Partitioning strategy**
	i找到大于pivot就停 j找到小于pivot就停，然后交换 **inplace**
	* 碰到等于pivot停不停？
		* 停！相对更equal size
		* 不停-->划分不对称-->worst case
* D. **Small Arrays**
	Problem：当划分至数据规模较小时，quick sort可能比insertion sort 慢
	Solution：在动态划分时，小于一定规模后cutoff改用quick sort
* E.**代码**

	![[Pasted image 20250514145401.png]]
	![[Pasted image 20250514145418.png]]
	（对left center right 排序，center即为pivot，swap至倒数第二位）
	（换至倒数第二位是因为已知最后一位已经大于pivot，不用参与排序）
	![[Pasted image 20250514145814.png]]
	（边界值很容易弄混！！！）
* F. **Analysis**
	![[Pasted image 20250514150425.png]]
	![[Pasted image 20250514150532.png]]
	![[Pasted image 20250514150633.png]]
	递归层数到一定规模后，可能转换为heap sort
	* ![[屏幕截图 2025-05-14 151344.png]]
		递归二选一即可，O（N）
* G. **Sorting Large Structure**
	![[Pasted image 20250514151733.png]]
	修改table的值（用list数组内的序号）以表示排序
	![[Pasted image 20250514152537.png]]
	【会有cycle--->需要temp来储存进入cycle第一个元素】
* H. **A General Lower Bound for Sorting**
	![[Pasted image 20250514153501.png]]
* I. Bucket Sort and Radix Sort
	 * Bucket Sort
	![[Pasted image 20250514154432.png]]
	  当M>>N时，空间利用率太低
	* Radix Sort
	![[Pasted image 20250514154713.png]]
	   按个十百位依次排序：
	    1.按个位数入桶
		2.按十位数入桶，入桶时按上一轮顺序依次入桶--->十位数相同，个位数小的数在前
		3.同理，百位数相同，后两位数小的数在前
	* 字典序
	![[Pasted image 20250514155115.png]]
	![[Pasted image 20250514155132.png]]
	![[Pasted image 20250514155151.png]]
	