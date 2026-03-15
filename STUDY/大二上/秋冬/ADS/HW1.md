An AVL tree is a self-balancing binary search tree. In an AVL tree, the heights of the two child subtrees of any node differ by at most one; if at any time they differ by more than one, rebalancing is done to restore this property. Figures 1-4 illustrate the rotation rules.
AVL树是一种自平衡二叉搜索树。在AVL树中，任何节点的两个子树的高度差最多为1；如果在任何时候它们的高度差超过1，就会进行重新平衡以恢复这一特性。图1-4说明了旋转规则。

![F1.jpg](https://images.ptausercontent.com/d265ae37-4348-4585-b39f-0b2e2e0a24f5.jpg)

![F2.jpg](https://images.ptausercontent.com/4a9f6fbd-e21e-4493-834d-7782e13bee4e.jpg)

![F3.jpg](https://images.ptausercontent.com/7dc0e66f-c458-4c92-bb8e-55b7bf6391ce.jpg)

![F4.jpg](https://images.ptausercontent.com/b17a9687-6be8-4256-873d-6a747154a58d.jpg)

Now given a sequence of insertions, you are supposed to tell the root of the resulting AVL tree.
现在给定一系列插入操作，你需要说出得到的AVL树的根节点。
### Input Specification:

Each input file contains one test case. For each case, the first line contains a positive integer N (≤20) which is the total number of keys to be inserted. Then N distinct integer keys are given in the next line. All the numbers in a line are separated by a space.
每个输入文件包含一个测试用例。对于每个测试用例，第一行包含一个正整数N（≤20），表示要插入的关键字总数。接下来一行给出N个不同的整数关键字。一行中的所有数字均用空格分隔。
### Output Specification:

For each test case, print the root of the resulting AVL tree in one line.
对于每个测试用例，在一行中输出得到的AVL树的根节点。
### Sample Input 1:

```in
5
88 70 61 96 120
```

### Sample Output 1:

```out
70
```

### Sample Input 2:

```
7
88 70 61 96 120 90 65
```

### Sample Output 2:

```
88
```



```
#include<stdio.h>
int main()
{
	
	return 0;
}
```


```
struct avltree
{
    int data;
    struct avltree *left;
    struct avltree *right;
    int height;
};
typedef struct avltree* node;
int height(node N) {
    if (N == NULL)
        return 0;
    return N->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

node createnode(int key) {
    node n = (node)malloc(sizeof(struct avltree));
    n->data = key;
    n->left = NULL;
    n->right = NULL;
    n->height = 1;
    return n;
}

node rr(node y) {
    node x = y->left;
    node T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

node lr(node x) {
    node y = x->right;
    node T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

int bf(node N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

node insert(node n, int key) {
    if (n == NULL)
        return createnode(key);

    if (key < n->data)
        n->left = insert(n->left, key);
    else if (key > n->data)
        n->right = insert(n->right, key);
    else
        return n;

    n->height = 1 + max(height(n->left), height(n->right));

    int bl = bf(n);

    if (bl > 1 && key < n->left->data)
        return rr(n);

    if (bl < -1 && key > n->right->data)
        return lr(n);

    if (bl > 1 && key > n->left->data) {
        n->left = lr(n->left);
        return rr(n);
    }

    if (bl < -1 && key < n->right->data) {
        n->right = rr(n->right);
        return lr(n);
    }

    return n;
}
```