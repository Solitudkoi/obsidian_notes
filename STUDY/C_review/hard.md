https://leetcode.cn/studyplan/top-100-liked/
- 哈希
- 双指针
- 滑动窗口
- 字串
- 普通数组
- 矩阵
- 链表
- 二叉树
- 图论
- 回溯
- 二分查找
- 栈
- 堆
- 贪心算法
- 动态规划
- 多位动态规划
- 技巧部分

```

基础 → 进阶
─────────────────────────────────
哈希 → 双指针 → 滑动窗口
  ↓
链表 → 二叉树 → 图论(BFS/DFS)
  ↓
二分查找 → 栈 → 堆
  ↓
回溯 → 贪心 → 动态规划


```


## 一、基础数据结构

### 1. 哈希

**核心**：O(1) 查找

```c
// C语言用数组模拟（范围小时）
int hash[100005] = {0};
hash[key] = value;

// 字符串哈希
int hash[256] = {0};
hash['a'] = 1;
```

**经典**：两数之和、最长无重复子串、字母异位词

---

### 2. 双指针

**核心**：两个指针同向/相向移动，O(n²) → O(n)

```c
// 相向双指针（有序数组）
int left = 0, right = n - 1;
while (left < right) {
    if (nums[left] + nums[right] == target) break;
    else if (nums[left] + nums[right] < target) left++;
    else right--;
}

// 快慢指针（链表找环）
while (fast && fast->next) {
    slow = slow->next;
    fast = fast->next->next;
    if (slow == fast) return true;
}
```

**经典**：三数之和、移动零、接雨水、链表找环

---

### 3. 滑动窗口

**核心**：维护一个动态窗口，右扩左缩

```c
int left = 0;
for (int right = 0; right < n; right++) {
    // 加入 right 元素
    while (窗口不满足条件) {
        // 移除 left 元素
        left++;
    }
    // 更新答案
}
```

**经典**：最小覆盖子串、长度最小的子数组、无重复字符的最长子串

---

## 二、线性结构

### 4. 子串 / 5. 数组 / 6. 矩阵

**核心技巧**：

- 前缀和：快速求区间和
- 差分数组：区间修改 O(1)
- 二维矩阵：坐标转换、螺旋遍历

```c
// 前缀和
prefix[0] = 0;
for (int i = 1; i <= n; i++)
    prefix[i] = prefix[i-1] + nums[i-1];
// 区间 [l, r] 的和 = prefix[r+1] - prefix[l];
```

---

### 7. 链表

**核心**：虚拟头节点 + 指针操作

```c
// 反转链表
ListNode* prev = NULL, *curr = head;
while (curr) {
    ListNode* next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
}
return prev;
```

**经典**：反转链表、合并有序链表、环形链表、LRU缓存

---

### 8. 二叉树

**核心**：递归 = 左子树 + 右子树 + 根

```c
// 前序遍历
void preorder(TreeNode* root) {
    if (!root) return;
    visit(root);
    preorder(root->left);
    preorder(root->right);
}

// 求最大深度
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + max(maxDepth(root->left), maxDepth(root->right));
}
```

**经典**：前中后序遍历、最大深度、最近公共祖先、层序遍历

---

## 三、图论与搜索

### 9. 图论

|算法|用途|时间复杂度|
|---|---|---|
|BFS|最短路（无权）|O(V+E)|
|DFS|路径、连通性|O(V+E)|
|并查集|连通分量|O(α(n))|
|Dijkstra|最短路（正权）|O(E log V)|
|拓扑排序|依赖关系|O(V+E)|

```c
// BFS 模板
queue.push(start);
visited[start] = 1;
while (!queue.empty()) {
    int u = queue.front(); queue.pop();
    for (邻居 v) {
        if (!visited[v]) {
            visited[v] = 1;
            queue.push(v);
        }
    }
}
```

---

### 10. 回溯

**核心**：试错 + 撤销选择

```c
void backtrack(路径, 选择列表) {
    if (满足结束条件) {
        存结果;
        return;
    }
    for (选择 : 选择列表) {
        做选择;
        backtrack(路径, 选择列表);
        撤销选择;  // 关键！
    }
}
```

**经典**：全排列、子集、组合总和、N皇后

---

## 四、优化算法

### 11. 二分查找

**核心**：有序数组 O(log n) 查找

```c
int left = 0, right = n - 1;
while (left <= right) {
    int mid = left + (right - left) / 2;  // 防溢出
    if (nums[mid] == target) return mid;
    else if (nums[mid] < target) left = mid + 1;
    else right = mid - 1;
}
return -1;
```

**经典**：搜索旋转数组、寻找峰值、x的平方根

---

### 12. 栈

**核心**：后进先出 LIFO

```c
int stack[10005], top = -1;
stack[++top] = x;    // push
top--;               // pop
stack[top];          // peek
```

**经典**：有效括号、每日温度、柱状图最大矩形

---

### 13. 堆（优先队列）

**核心**：快速获取最大/最小值

```c
// C语言手写小顶堆
int heap[10005], size = 0;

void push(int x) {
    heap[++size] = x;
    for (int i = size; i > 1 && heap[i] < heap[i/2]; i /= 2)
        swap(&heap[i], &heap[i/2]);
}

int pop() {
    int res = heap[1];
    heap[1] = heap[size--];
    // 下沉...
    return res;
}
```

**经典**：前K个高频元素、合并K个链表、中位数

---

## 五、动态规划

### 14. 贪心

**核心**：局部最优 → 全局最优 **经典**：跳跃游戏、区间调度、分发糖果

---

### 15. 动态规划

**五步法**：

1. 定义状态 `dp[i]` 的含义
2. 找状态转移方程
3. 初始化边界
4. 确定遍历顺序
5. 举例推导

```c
// 一维：爬楼梯
dp[i] = dp[i-1] + dp[i-2];

// 二维：0-1背包
for (int i = 1; i <= n; i++) {
    for (int j = W; j >= w[i]; j--) {
        dp[j] = max(dp[j], dp[j-w[i]] + v[i]);
    }
}
```

**经典**：爬楼梯、背包问题、最长递增子序列、最大子数组和

---

### 16. 多维动态规划

```c
// 最长公共子序列
for (int i = 1; i <= m; i++)
    for (int j = 1; j <= n; j++)
        if (s1[i-1] == s2[j-1])
            dp[i][j] = dp[i-1][j-1] + 1;
        else
            dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
```

**经典**：编辑距离、不同路径、最大正方形

---

## 六、技巧

### 17. 技巧部分

|技巧|用途|
|---|---|
|位运算|状态压缩、找唯一数|
|异或|找出现一次的数|
|原地交换|不用额外空间|

```c
// 交换两数
a ^= b; b ^= a; a ^= b;

// 找只出现一次的数
int res = 0;
for (int i = 0; i < n; i++) res ^= nums[i];
```

