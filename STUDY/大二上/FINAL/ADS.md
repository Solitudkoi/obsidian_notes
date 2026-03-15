- 2015-2016
![[Pasted image 20260102105712.png]]


![[Pasted image 20260102111508.png]]

![[Pasted image 20260102112626.png]]

![[Pasted image 20260102113008.png]]

**题目**：操作 `insert`（代价 1）、`sum`（代价为列表长度，合并为一个元素），潜在函数 Φ=元素总数，哪个陈述为假？

摊还代价公式：ci​^​=ci​+Φi​−Φi−1​

**选项解析**：

1. **选项 1**：insert 的实际代价是 1 → 正确。题目明确给出 insert 代价为 1。
2. **选项 2**：insert 的潜在变化是 1，摊还代价是 2 → 正确。insert 后元素数 + 1 → Φi​−Φi−1​=1 → c^=1+1=2。
3. **选项 3**：sum 的实际代价是 k（列表长度为 k） → 正确。题目明确 sum 代价为列表长度。
4. **选项 4**：sum 的潜在变化是 2−k，摊还代价是 2 → **错误（本题答案）**。sum 操作将 k 个元素合并为 1 个 → 潜在变化 Φi​−Φi−1​=1−k → 摊还代价 c^=k+(1−k)=1，不是 2。

![[Pasted image 20260102131625.png]]

- 2016-2017

![[Pasted image 20260102141225.png]]

![[Pasted image 20260102143154.png]]
![[Pasted image 20260102153156.png]]

```
int min(int x, int y){
    return x<y? x: y;
}

int max(int x, int y){
    return x<y? y: x;
}

int need_time(const int time[], const int score[], int happy_score, int n){
    int total_score = 0, total_time = 0, min_score = 10000;
    

    for(int i = 0; i < n; i++){
        total_time += time[i];
        total_score += score[i];
        min_score = min( min_score, score[i]);
    }
    if(total_score < happy_score) return -1;

    int left_score = total_score - happy_score;
    int dp[left_score + 1][n+1];

    
    for(int i = 0; i<=left_score; i++){
        for(int j = 0; j<=n; j++){
            dp[i][j] = 0;
        }
    }
    

    for(int i = min_score; i<=left_score; i++){
        for(int j = 1; j<=n; j++){
            if(i < score[j-1]){
                dp[i][j] = dp[i][j-1];
            }else{
                dp[i][j] = max(time[j-1] + dp[i-score[j-1]][j-1], dp[i][j-1]);
            }
        }
    }

    return (total_time != dp[left_score][n])? total_time - dp[left_score][n] : -1;

}
```


- HW
![[Pasted image 20260102195043.png]]

![[Pasted image 20260102195149.png]]
![[Pasted image 20260102195213.png]]

![[Pasted image 20260102202713.png]]
![[Pasted image 20260102202730.png]]

DP
```
#include<stdio.h>
#define inf -1000000000
int main()
{
    int n,m;
    scanf("%d%d",&n,&m);
    int bread[1005]={0},cream[1005]={0};
    for(int i=1;i<=n;i++)    scanf("%d",&bread[i]);
    for(int i=1;i<=m;i++)    scanf("%d",&cream[i]);
    int total=inf;
    static int DP[1005][1005];
    for(int i=0;i<=n;i++)
    {
        for(int j=0;j<=m;j++)
            DP[i][j]=inf;
    }
    for(int j=0;j<=m;j++)    DP[0][j]=0;
    for(int j=0;j<=n;j++)    DP[j][0]=0;
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=m;j++)
        {
            if(DP[i][j-1]>DP[i-1][j])
                DP[i][j]=DP[i][j-1];
            else DP[i][j]=DP[i-1][j];
            if(DP[i-1][j-1]!=inf)
            {
                if(DP[i-1][j-1]+bread[i]*cream[j]>DP[i][j])
                    DP[i][j]=DP[i-1][j-1]+bread[i]*cream[j];
            }
        }
    }
    for(int j=1;j<=m;j++)
    {
        if(DP[n][j]>total)    total=DP[n][j];
    }
    printf("%d\n",total);
    return 0;
}
```

![[Pasted image 20260103101728.png]]
![[Pasted image 20260103101747.png]]
![[Pasted image 20260103102201.png]]
![[Pasted image 20260103102215.png]]
![[Pasted image 20260103102243.png]]
![[Pasted image 20260103102303.png]]
![[Pasted image 20260103102323.png]]


HW 14单选+HW9还没看
以及还有5张历年卷


![[Pasted image 20260107093425.png]]

![[Pasted image 20260107115439.png]]


Which one of the following statements is FALSE?

A.For red-black trees, the total cost of rebalancing for m consecutive insertions in a tree of n nodes is O(n+m).

B.To obtain O(1) armortized time for the function **decrease-key**, the potential function used for Fibonacci heaps is Φ(H)=t(H)+m(H), where t(H) is the number of trees in the root list of heap H, and m(H) is the number of marked nodes in H.

C.Let S(x) be the number of descendants of x (x included). If the potential function used for splay tree is Φ(T)=∑x∈T​logS(x) , then the amortized cost of one splay operation is O(logn).

D.In the potential method, the amortized cost of an operation is equal to the actual cost plus the increase in potential due to this operation.

参考答案 B

---

Which one of the following statements is FALSE?

A.SAT, Vertex Cover, Hamiltonian Cycle, Clique, Knapsack, Bin Packing, and Domination Set problems are all NP-completeness problems.

B.If there is a polynomial time (1+2n1​)-approximation algorithm for Vertex Cover with n being the total number of vertices in the graph, then P=NP.

C.If there is a polynomial time 3/2-approximation algorithm for K-Center, then P=NP.

D.Given a weighted directed acyclic graph (DAG) G and a source vertex s in G, it is NP-hard to find the longest distances from s to all other vertices in G.

参考答案 D

---

Which one of the following statements is FALSE?

A.A language L1​ is polynomial time transformable to L2​ if there exists a polynomial time function f such that w∈L1​ if f(w)∈L2​.

B.L1​≤p​L2​ and L2​≤p​L3​ then L1​≤p​L3​.

C.If L1​∈P then L1​⊆NP∩ co-NP.

D.If language L1​ has a polynomial reduction to language L2​, then the complement of L1​ has a polynomial reduction to the complement of L2​.

参考答案  A




```
int FindMinSteps(int N, int K)
{
    if(N==1)  return 0;
    int dist[1000001]; 
    int queue[1000001];
    for (int i=0;i<=N;i++) dist[i]=-1;
    int head=0,tail=0;
    queue[tail++] = 1;
    dist[1] = 0;
    while (head < tail) 
    {
        int curr = queue[head++]; // 从名单里取出一个数
        int current_step = dist[curr];
        if (K > 1) 
        {
            long long next = (long long)curr * K;
            if (next <= N && dist[next] == -1) 
            {
                dist[next] = current_step + 1;
                queue[tail++] = (int)next;
                if (next == N) return dist[next]; // 找到了直接返回
            }
        }
        for (int t = 1; t <= 10; t++) 
        {
            int next = curr + t;
            if (next <= N && dist[next] == -1) 
            {
                dist[next] = current_step + 1;
                queue[tail++] = next;
                if (next == N) return dist[next]; // 找到了直接返回
            }
        }
    }
    return dist[N];
}
```


```
BinQ DeleteRt( BinQ H, int Pos )
{
    BinTree OldRoot, SubTree;
    BinQ NewBinQ;
    int p;
    
    OldRoot = H->TheTrees[Pos];
    SubTree = OldRoot->LeftChild;
    free(OldRoot);
    NewBinQ = Initialize();
    NewBinQ->CurrentSize = (1<<Pos)-1;
    for ( p = Pos - 1; p >= 0; p-- ) {
        NewBinQ->TheTrees[p] = SubTree;
        SubTree = SubTree->NextSibling;
        NewBinQ->TheTrees[p]->NextSibling = NULL;
    }
    return NewBinQ;
}
```


```
#include<stdio.h>
#define inf -1000000000
int main()
{
    int n,m;
    scanf("%d%d",&n,&m);
    int bread[1005]={0},cream[1005]={0};
    for(int i=1;i<=n;i++)    scanf("%d",&bread[i]);
    for(int i=1;i<=m;i++)    scanf("%d",&cream[i]);
    int total=inf;
    static int DP[1005][1005];
    for(int i=0;i<=n;i++)
    {
        for(int j=0;j<=m;j++)
            DP[i][j]=inf;
    }
    for(int j=0;j<=m;j++)    DP[0][j]=0;
    for(int j=0;j<=n;j++)    DP[j][0]=0;
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=m;j++)
        {
            if(DP[i][j-1]>DP[i-1][j])
                DP[i][j]=DP[i][j-1];
            else DP[i][j]=DP[i-1][j];
            if(DP[i-1][j-1]!=inf)
            {
                if(DP[i-1][j-1]+bread[i]*cream[j]>DP[i][j])
                    DP[i][j]=DP[i-1][j-1]+bread[i]*cream[j];
            }
        }
    }
    for(int j=1;j<=m;j++)
    {
        if(DP[n][j]>total)    total=DP[n][j];
    }
    printf("%d\n",total);
    return 0;
}
```

```
int FindMinSteps(int N, int K)
{
    if(N==1)  return 0;
    int dist[1000001]; 
    int queue[1000001];
    for (int i=0;i<=N;i++) dist[i]=-1;
    int head=0,tail=0;
    queue[tail++] = 1;
    dist[1] = 0;
    while (head < tail) 
    {
        int curr = queue[head++]; // 从名单里取出一个数
        int current_step = dist[curr];
        if (K > 1) 
        {
            long long next = (long long)curr * K;
            if (next <= N && dist[next] == -1) 
            {
                dist[next] = current_step + 1;
                queue[tail++] = (int)next;
                if (next == N) return dist[next]; // 找到了直接返回
            }
        }
        for (int t = 1; t <= 10; t++) 
        {
            int next = curr + t;
            if (next <= N && dist[next] == -1) 
            {
                dist[next] = current_step + 1;
                queue[tail++] = next;
                if (next == N) return dist[next]; // 找到了直接返回
            }
        }
    }
    return dist[N];
}
```


![[Pasted image 20260107193940.png]]

``
```
分数 10

Is it a B+ tree?

作者 刘金飞

单位 浙江大学

The teacher wants to write the `IsBpT` function to check if the trees submitted by students satisfy the definition of the B+ tree of a given order (e.g., order 4) learned in our class. The B+ tree structure is defined as follows:

```c
typedef struct BpTNode BpTNode; 
struct BpTNode { 
    bool isLeaf; /* 1 if this node is a leaf, or 0 if not */
    bool isRoot; /* 1 if this node is the root, or 0 if not */
    BpTNode** children; /* Pointers to children. This field is not used by leaf nodes. */ 
    ElementType* keys;
    int num_children; /* Number of valid children (not NULL) */
    int num_keys; /* Number of valid keys */
};
```c

Fortunately, the students are all brilliant, so the B+ trees they submit guarantee to meet the following properties:

1. There is a root node, and all leaf nodes are at the same depth;
    
2. The key values stored in all leaf nodes are arranged in strictly ascending order from left to right.
    

Your task is to complete the function `IsBpT` as follows so that the teacher can determine whether a tree submitted by a student meets the other properties required by the definition of the B+ tree of a given order. Return `true` if the tree is a B+ tree, or `false` if not.

```c
bool IsBpT(BpTNode* node, int order) {
    if (node->isLeaf == 1) { /* this is a leaf node */
        if (node->isRoot == 1) { /* this tree has only one node */
            if (node->num_keys < 1 || node->num_keys > order) return false;
        }
        else {
            if (node->num_keys < (order + 1) / 2 || node->num_keys > order) return false;
        }
    }
    else {
        /* check the property of the tree structure */
        if (node->num_keys != node->num_children - 1) return false;
        if (node->isRoot == 1) { /* this is the root node */
            if (node->num_keys < 1 || node->num_keys > order - 1) return false;
            else if (node->num_children < 2 || node->num_children > order) return false;
        }
        else {
            if ( || node->num_keys > order - 1) return false;
            else if (node->num_children < (order + 1) / 2 || node->num_children > order) return false;
        }

        /* check the property of the value of key */
        for (int i = 0; i < node->num_keys; i++) {
            BpTNode* key_node = ;
            while (key_node->isLeaf == 0) {
                key_node = key_node->children[0];
            }
            if (node->keys[i] != key_node->keys[0]) return false;
        }
        for (int i = 0; i < node->num_children; i++) {
            if (IsBpT(node->children[i], order) == false) return false;
        }
    }
    return true;
}
```c
```
