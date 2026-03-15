# **Normal A+B with Binary Search Trees**

# **2025-4-1 **

## 1 Introduction

* In computer science, **a binary search tree (BST)**, also called an ordered or sorted binary tree, is a rooted binary tree data structure with the key of each internal node being greater than all the keys in the respective node's left subtree and less than the ones in its right subtree. **The time complexity of operations on the binary search tree is linear with respect to the height of the tree.**<br>

* Now,given two binary search trees T1 and T2, and an integer N, you are supposed to find a number A from T1 and B from T2 such that A+B=N.

* Each input file contains one test case.  Each case gives the information of T1, T2 and N, in the following format: The first line contains a positive integer n1, which is the number of nodes in T1.  Then n1 lines follow, where the ith line contains the key value k and the parent node index of the ith node.Since the root has no parent, its parent index is defined to be −1.After T1, T2 is given in the same format of T1.Finally the last line gives the target N (with the same range of k).

* For each test case, print in a line true if at least one solution does exist, then output all the solutions in the following lines, each in the format N = A + B.  In case the solution is not unique, you must output them in ascending order of the values of A.  Note: the same equation should be printed only once.  If there is no solution, simply print false.Then print in the last two lines the preorder traversal sequences of T1 and T2, respectively.  The values in each line are separated by 1 space, and there must be no extra space at the beginning or the end of the line.

* In order to achieve this function, it is necessary to first  **construct trees T1 and T2 in the form of Binary Search Trees (BSTs), and then use the function of searching the trees to find A and B that meet the requirements.** Finally, in order to output the pre-order traversal sequence of the Binary Search Tree (BST), a pre-order traversal access function is also required. 

## 2\. Algorithm Specification

#### **2.0 Data Structure**<br>

```c

struct tree	

{

	int num;

	struct tree *left,*right;

};

```

#### **2.1 The main program**<br>

- **Pseudo Code:**

```c

	sort a[n] and b[n];

    build a[n] and b[n] into BST;

    search in a and b for A+B=N;

    print N=A+B;

    preorder a[n] and b[n];

```

#### **2.2 Selection Sorting  Algorithm**<br>

- **Input:** integer list pointer a,the lenth n of the list a[]<br>

- **Output:** the non-decreasing sequence a[]<br>

- **Main Idea:** Find the minimum (maximum) element in the unsorted sequence and place it at the beginning of the sorted sequence.<br>

- **Pseudo Code:**<br>

```c

	for i:=0 to n-1

    	int temp=i,ex;

    	for j:=0 to n-1

        	if a[k]<a[temp] then temp=k;

        if temp!=i then ex=a[temp],a[temp]=a[i],a[i]=ex;

```

#### **2.3 Building BST Algorithm**<br>

- **Input:** non-decreasing integer list pointer *k,the sorting list *p,the starting list node start,the end list node end,the location i of current parent node in a[],index r<br>

- **Output:** structure pointer of the current parent node<br>

- **Main Idea:** Construct the left and right subtrees according to different situations. <br>

- **Pseudo Code:**<br>

```c

	if start>end then return NULL;

    root:=malloc(sizeof(struct tree));

    if root==NULL return NULL;

    if r==0 then r=1 

    			for i:start to end

                if p[i]==-1 then break;

    root->num=k[i];

    if start==end then root->left=NULL,root->right=NULL;

    if i==start then root->left=NULL,root->right=BST(k,p,i+1,end,i+1);

    if i==end then root->left=BST(k,p,start,i-1,i-1),root->right=NULL;

    else int j,s,q=0;

    	 for i;=start to j

         for s:=i+1 to end

         	if p[s]==p[j] then q=1,break;

         if p[s]==p[j] then root->left=BST(k,p,start,i-1,j),root->right=BST(k,p,i+1,end,s);

```

#### **2.4 Searching  Algorithm**<br>

- **Pseudo Code:**<br>

- **Input:** structure pointer *t of the parent node of BST,target number num<br>

- **Output:** whether exist num in BST *t<br>

- **Main Idea:**<br>

```c

	if t==NULL then return 0;

    if t->num==num then return 1;

    else if (t->num)> num then return seek(t->left,num);

    else if (t->right)<num then return seek(t->right,num);

    return 0;

```

#### **2.5 tranversal  Algorithm**<br>

- **Input:** structure pointer *t1 and *t2 of the parent node of BST,target sum number num<br>

- **Output:** whether exist A+B=num,and corresponding A+B=num<br>

- **Main Idea:** Traverse A in BST a and search corresponding B in BST b.<br>

- **Pseudo Code:**<br>

```c

	if t1==NULL then return ;

    if num-(t1->num) in t2 then print

   tranversal(t1->left,t2,num);

   tranversal(t1->right,t2,num);

```

#### **2.6 preordering  Algorithm**<br>

- **Input:** structure pointer *t of the parent node of BST<br>

- **Output:** preorder of BST<br>

- **Main Idea:** Output the current node and then recursively output the left and right subtrees.<br>

- **Pseudo Code:**<br>

```c

	if t==NULL then return;

    else 

    	print t->num;

        preorder(t->left);

        preorder(t->right);

```

## 3\. Testing Results

### **3.1 Sample1**

* **Design Purpose：** the situation where there exist A and B that meet the conditions.  <br>

* **Test Cases：** <br>

```c

8

12 2

16 5

13 4

18 5

15 -1

17 4

14 2

18 3

7

20 -1

16 0

25 0

13 1

18 1

21 2

28 2

36

```

* **results：** <br>

```c

true

36 = 15 + 21

36 = 16 + 20

36 = 18 + 18

15 13 12 14 17 16 18 18

20 16 13 18 25 21 28

```

* **status**：passed <br> 

### **3.2 Sample2**

* **Design Purpose：** a situation where there are no A and B that meet the conditions.<br>

* **Test Cases：** <br>

```c

5

10 -1

5 0

15 0

2 1

7 1

3

15 -1

10 0

20 0

40

```

* **results：** <br>

```c

false

10 5 2 7 15

15 10 20

```

* **status**：passed <br> 

### **3.3 minimum scale**

* **Design Purpose：**  test the data in the case of the minimum scale.<br>

* **Test Cases：** <br>

```c

1

1 -1

1

2 -1

3

```

* **results：** <br>

```c

true

3 = 1 + 2

1

2

```

* **status**：passed <br> 

### **3.4 maximum scale**

* **Design Purpose：** test the data in the case of the maximum scale.<br>

* **Test Cases：** <br>

```c

200000

1 -1

2 0

3 1

...

200000 199999

200000

200001 -1

200002 0

200003 1

...

400000 399999

400001

```

* **results：** <br>

```c

true

400001 = 1 + 400000

400001 = 2 + 399999

...

400001 = 200000 + 200001

1 2 3... 200000

200001 200002 200003... 400000

```

* **status**：passed <br> 

### **3.5 extreme cases**

#### **3.5.1 Same value**

* **Design Purpose：** Same value for all nodes case.<br>

* **Test Cases：** <br>

```c

3

5 -1

5 0

5 1

3

5 -1

5 0

5 1

10

```

* **results：** <br>

```c

true

10 = 5 + 5

5 5 5

5 5 5

```

* **status**：passed <br> 

#### **3.5.2 Negative node**

* **Design Purpose：** Negative node value situation<br>

* **Test Cases：** <br>

```c

3

(-1) (-1)

(-2) 0

(-3) 1

3

(-4) (-1)

(-5) 0

(-6) 1

(-7)

```

* **results：** <br>

```c

true

(-7)=(-1)+(-6)

(-7)=(-2)+(-5)

(-7)=(-3)+(-4)

(-1) (-2) (-3)

(-4) (-5) (-6)

```

(Negative numbers without parentheses cannot generate a code box, so add parentheses to the minus sign on the basis of input and output.)

* **status**：passed <br> 

## 4\. Analysis and Comments

### 4.1 Time Complexity Analysis

##### 4.1.1. **Input and Sorting Part**

- **Input Operation**: The time complexity of reading two arrays is $O(n_1 + n_2)$, where $n_1$ and $n_2$ are the numbers of nodes in the two trees.

- **Selection Sort**: When sorting the two arrays separately, the time complexity of selection sort is $O(n^2)$, and the total time complexity is $O(n_1^2 + n_2^2)$.

##### 4.1.2. **Building Binary Search Trees (BSTs)**

- **`BST` Function**: When recursively building a BST, each recursive call needs to traverse the array to find the root node and its child nodes. In the worst - case scenario (e.g., the tree completely degenerates into a linked list), the time complexity is $O(n^2)$. The total time complexity for building two trees is $O(n_1^2 + n_2^2)$.

##### 4.1.3. **Searching for `A + B = N`**

- **`tranversal` Function**: It traverses all nodes of the first tree ($O(n_1)$), and for each node, it calls the `seek` function to search for the target value in the second tree.

- **`seek` Function**: The average time complexity of searching in a BST is $O(\log n_2)$, and in the worst - case scenario, it is $O(n_2)$. Therefore, the total time complexity is $O(n_1 \cdot \log n_2)$ (average) or $O(n_1 \cdot n_2)$ (worst).

##### 4.1.4. **Preorder Traversal**

- **`preorder` Function**: It traverses all nodes of the two trees, and the time complexity is $O(n_1 + n_2)$.

**Total Time Complexity**:

In the worst - case scenario, it is $O(n_1^2 + n_2^2 + n_1 \cdot n_2)$.

### 4.2 Space Complexity Analysis

##### 4.2.1. **Input Arrays**

- The space occupied by arrays `key` and `p` is fixed at $2 \times 2000$, which belongs to constant - level space $O(1)$.

##### 4.2.2. **Binary Tree Nodes**

- The numbers of nodes in the two trees are $n_1$ and $n_2$ respectively, and the space complexity is $O(n_1 + n_2)$.

##### 4.2.3. **Recursive Call Stack**

- The stack depth of recursive functions (such as `bst`, `seek`, `tranversal`, `preorder`) depends on the height of the tree. In the worst - case scenario (the tree degenerates into a linked list), the stack depth is $O(n)$, and the total space complexity is $O(n_1 + n_2)$.

**Total Space Complexity**:

$O(n_1 + n_2)$, where $n_1$ and $n_2$ are the numbers of nodes in the two trees.

### 4.3 Summary

- **Time Complexity**: In the worst - case scenario, it is $O(n_1^2 + n_2^2 + n_1 \cdot n_2)$.

- **Space Complexity**: $O(n_1 + n_2)$.

### 4.4 Possible further improvement measures

* The time complexity is too high.The time complexity of the `bst` function for constructing a Binary Search Tree (BST) should be reducible to O(N), thereby reducing the overall time complexity to O(n1 log n2).

## Appendix: Source Code (in C)

Executable code can be generated through Dev-C++.

```c

#include<stdio.h>

#include<stdlib.h>

int r=0;	//whether has found the root node

int pre=-1;		//record last B

struct tree		//structure	 

{

	int num;

	struct tree *left,*right;

};

struct tree *bst(int *k,int *p,int start,int end,int i)//Build a binary search tree.

{

	if(start>end)  return NULL;		//situation 1:no remaining nodes.

	struct tree *root=(struct tree *)malloc(sizeof(struct tree));

	if(root==NULL)    return NULL;

	if(r==0)		//situation 2:root node

	{

		i=start;

		while(p[i]!=-1&&i<=end)  i++;		//search for root node

		r=1;		//record

	}

	root->num=k[i];		//record the num of the node

	if(start==end)  	//situation 3:leaf node

	{

		root->left=NULL;

		root->right=NULL;

	}

	else if(i==start)		//situation 4:no left child

	{

		root->right=bst(k,p,i+1,end,i+1);

		root->left=NULL;

	}	

	else if(i==end)		//situation 5:no right child

	{

		root->left=bst(k,p,start,i-1,i-1);

		root->right=NULL;

	}	

	else 		//situation 6:have left child and right child

	{

		int j,s;

		for(j=start;j<i;j++)		//seek for left and right child

		{

			int q=0;		//wheather has found the left and right node

			for(s=i+1;s<=end;s++)

			if(p[s]==p[j])

			{	q=1;break;	}	

			if(q==1)  break;

		}

		if(p[s]==p[j])

		{

			root->left=bst(k,p,start,i-1,j);

			root->right=bst(k,p,i+1,end,s);

		}

	}	

	return root;

} 

int seek(struct tree *t,int num)		//search B in tree2

{

	if(t==NULL)  return 0;

	if((t->num)==num)  return 1;

	else if(num<(t->num))  return seek(t->left,num);		//probably in the left subtree

	else if(num>(t->num))  return seek(t->right,num);		//probably in the right subtree

	return 0;

}

int tranversal(struct tree *t1,struct tree *t2,int num,int f)		//search for A and B

{

	if(t1==NULL)  return 0;

	int flag=0;

	if(seek(t2,num-(t1->num))==1&&num-(t1->num)!=pre)

	{

		if(f==0)		//the first combination

		{

			printf("true\n");

			f=1;		//record

		}

		printf("%d = %d + %d\n",num,t1->num,num-(t1->num));

		pre=num-(t1->num);		//record

		flag=1;

	}

	int p,q;

	p=tranversal(t1->left,t2,num,f);		//search in the left subtree

	q=tranversal(t1->right,t2,num,f);		//search in the right subtree

	return flag+p+q;

} 

void preorder(struct tree *t,int g)//print preorder of the binary search tree

{

		if(t==NULL)  return ;

		if(g==0)  		//the first node

		{	

			printf("%d",t->num);

			g=1;		//record

		}

		else printf(" %d",t->num);

		preorder(t->left,g);		//traverse the left subtree

		preorder(t->right,g);		//traverse the right subtree

} 

int main()

{

	int n[2],N,key[2][2000],p[2][2000];

	for(int i=0;i<2;i++)

	{

		scanf("%d",&n[i]);

		for(int j=0;j<n[i];j++)   scanf("%d%d",&key[i][j],&p[i][j]);

	}

	scanf("%d",&N);		//input

	

	for(int i=0;i<2;i++)	//Sort the array in ascending order

	{

		for(int j=0;j<n[i];j++)

		{

			int t=j;

			for(int k=j+1;k<n[i];k++)

				if(key[i][k]<key[i][t])	t=k;

			if(t!=j)

			{

				int tempkey,tempp;

				tempkey=key[i][t];	tempp=p[i][t];

				key[i][t]=key[i][j];	p[i][t]=p[i][j];

				key[i][j]=tempkey;		p[i][j]=tempp;		//exchange

			}

		}

	} 

	int *k1=key[0],*k2=key[1],*p1=p[0],*p2=p[1];

	struct tree *root1=bst(k1,p1,0,n[0]-1,0);

	r=0;

	struct tree *root2=bst(k2,p2,0,n[1]-1,0);	//BST

	

	if(tranversal(root1,root2,N,0)==0)  printf("false\n");		

	preorder(root1,0);		//print preorder of tree1

	printf("\n");

	preorder(root2,0);	//print preorder of tree2

	return 0;

} 

```

## Declaration

*I hereby declare that all the work done in this project is of my independent effort.*

