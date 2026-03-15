# Dijkstra Sequence

# 2025.4.25 蒋玥

## 1 Introduction

#### 1.1Background

* Dijkstra's algorithm is an algorithm for **finding the shortest paths between nodes in a weighted graph**.The algorithm finds the shortest path from a given source node to every other node. It can be used to find the shortest path to a specific destination node, **by terminating the algorithm after determining the shortest path to the destination node**.The algorithm uses a min-priority queue data structure for selecting the shortest paths known so far.

#### 1.2Problem description

* Now,given a graph and a sequence,our job is to check whether a given sequence is Dijkstra sequence or not.

* For each test case, the input consists of: two positive integers denoting the number of vertices and the number of edges, followed by Ne lines of edge information (each containing the indices of the two end vertices and a positive integer weight, with the graph guaranteed to be connected). Finally, a positive integer K indicates the number of queries, with each of the subsequent K lines being a permutation of Nv vertices (where the first vertex of each permutation is the source). All input elements are separated by spaces.

#### 1.3 Solution Overview

* The key to determining a Dijkstra sequence is that **each vertex (excluding the source vertex) must have the shortest current distance to the source among the remaining vertices when it is added**. If there are multiple vertices with the same distance, their order can be arbitrary, but they must be selected when they first become the minimum distance.

* The algorithmic approach is as follows: First, for the source vertex of each sequence, use Dijkstra's algorithm to pre - compute the shortest distances from the source to all other vertices. Then, iterate through the vertices in the sequence. By maintaining a set of selected vertices, verify step - by - step whether each vertex has the minimum distance among the remaining vertices when it is added. Also, ensure that vertices with the same distance have been selected before it, thereby determining whether the sequence is a valid Dijkstra sequence.

## 2 Algorithm Specification

#### 2.0 Data Structure<br>

```c

struct AdjVnode // node structure

{

int vertex;

int weight;

struct AdjVnode *next;

};

typedef struct Vnode //The structure of adjacent nodes of a node.

{

struct AdjVnode *firstedge;

} *AdjList[MAXVERTEX];

struct Gnode //the structure of the graph

{

int nv;

int ne;

AdjList G;

};

struct heapnode //structure of the heap node

{

int vertex;

int weight;

};

struct heap //the structure of the heap

{

struct heapnode *T[MAXVERTEX];

int size;

int cap;

};

```

#### 2.1 The main program<br>

- **Pseudo Code:**

```c

input nv, ne

Initialize Graph

for i from 0 to ne:

input r, c, w

BuildGraph(Graph,r,c,w)

input num

for i from 0 to num:

s = new array[nv + 1]

for j from 1 to nv:

input s[j]

if judge(weight,s,nv) == 1:

output "Yes"

else:

output "No"

free s

free weight

return 0

```

#### 2.2 function1 BuildGraph<br>

- **Input: the Graph , vertex r and c, weight w**<br>

- **Main Idea: build edges from r to c and from c to r in the Graph** <br>

- **Pseudo Code:**<br>

```c

// Create edge r -> c

s=ALLOCATE(AdjVnode)



Graph.G[r].firstedge=s

// Create edge c -> r (undirected graph)

t=ALLOCATE(AdjVnode)

t.vertex=r

t.weight=w

t.next=Graph.G[c].firstedge

Graph.G[c].firstedge=t

```

#### 2.3 function2 InsertHeap<br>

- **Input: heap H, integer vertex and weight**<br>

- **Output: the heap with vertices already inserted**<br>

- **Main Idea: after inserting the vertex at the end of the heap, perform the percolate-up operation** <br>

- **Pseudo Code:**<br>

```c

H.size=H.size + 1

idx=H.size - 1

H.T[idx]=ALLOCATE heapnode

H.T[idx].vertex=vertex

H.T[idx].weight=weight

i=idx

while i>0 and H.T[(i-1)/2].weight>weight

{

H.T[i].vertex=H.T[(i-1)/2].vertex

H.T[i].weight=H.T[(i-1)/2].weight

i=(i-1) / 2

}

H.T[i].vertex=vertex

H.T[i].weight=weight

return H

```

#### 2.4 function3 selectmin<br>

- **Input: heap H, integer list[][]**<br>

- **Output: the node with the minimum path distance to the known region**<br>

- **Main Idea: utilize the properties of the heap to search** <br>

- **Pseudo Code:**<br>

```c

if H->size==0 then return MAXWEIGHT

min=MAXWEIGHT

adr=-1

for i:=0 to H->size-1

if list[H->T[i]->vertex][1]==0&&H->T[i]->weight<min then

min=H->T[i]->weight;

adr=i;

if adr==-1 then return MAXWEIGHT

temp=H->T[adr]

H->T[adr]=H->T[H->size-1]

H->size--

i=adr

while 2*i+1<H->size

left=2*i + 1

right=2*i + 2

smallest=left

if right<H->size and H->T[right]->weight<H->T[left]->weight then

smallest=right

if H->T[i]->weight<=H->T[smallest]->weight then

break

swap H->T[i] and H->T[smallest]

i=smallest

}

return min

```

#### 2.5 function4 judge<br>

- **Input: the graph Graph, the sequence s[], the number of vertex nv**<br>

- **Output: 1/0 representing the sequence is Dijkstra Sequence or not**<br>

- **Main Idea: Traversing the sequence, set the first vertex path, validate others, update adjacent paths, find the next min - path vertex, mark the current one visited.** <br>

- **Pseudo Code:**<br>

```c

list=new array[nv + 1][2]

Initialize list

pm=maxweight

for i from 1 to nv:

min=maxweight

if i==1:

list[s[i]][0]=0

else if list[s[i]][0]>pm:

free list

return 0

for j from 1 to nv:

if weight[s[i]][j]!=0 and weight[s[i]][j]+list[s[i]][0]<list[j][0] and list[j][1]==0:

list[j][0]=weight[s[i]][j]+list[s[i]][0]

for k from 1 to nv:

if list[k][0]<min and list[k][1]==0 and k!=s[i]:

min=list[k][0]

pm=min

list[s[i]][1]=1

free list

return 1

```

## 3 Testing Results

### 3.1 Sample

* **Test Cases：** <br>

```c

5 7

1 2 2

1 5 1

2 3 1

2 4 1

2 5 2

3 5 1

3 4 1

4

5 1 3 4 2

5 3 1 2 4

2 3 4 5 1

3 2 1 5 4

```

* **Results：** <br>

```c

Yes

Yes

Yes

No

```

* **Status**：passed <br>

### 3.2 Minimum Scale (Single-Source Two-Vertex Graph)

* **Design Purpose：** The case with the minimum scale <br>

* **Test Cases：** <br>

```c

2 1

1 2 5

1

1 2

```

* **Results：** <br>

```c

Yes

```

* **Status**：passed <br>

### 3.3 Maximum Scale

* **Design Purpose：** The case close to the maximum scale<br>

* **Test Cases：** <br>

```c

1000 100000

1 2 1

1 3 1

... (99998 edges omitted, forming a star graph with 1 as the center, all edges weight 1)

999 1000 1

100

1 2 3 4 ... 1000 (Valid: ordered by distance)

2 1 3 4 ... 1000 (Invalid: source must be first)

1 3 2 4 5 ... 1000 (Valid: 2 and 3 have equal distance, order interchangeable)

```

* **Results：** <br>

```c

Yes

No

Yes

...

```

* **Status**：passed <br>

### 3.4 Multiple Vertices with Equal Distances (Order Varies)

* **Design Purpose：** The case with multiple Dijkstra Sequences<br>

* **Test Cases：** <br>

```c

3 3

1 2 5

1 3 5

2 3 1

2

1 2 3

1 3 2

```

***Results：** <br>

```c

Yes

Yes

```

* **Status**：passed <br>

### 3.5 Edge Case

* **Design Purpose：** The case where the distances from the source vertex to all other vertices are equal

<br>

* **Test Cases：** <br>

```c

4 6

1 2 10

1 3 10

1 4 10

2 3 1

2 4 1

3 4 1

3

1 2 3 4

1 4 3 2

1 2 4 3

```

* **Results：** <br>

```c

Yes

Yes

Yes

```

* **Status**：passed <br>

### 3.6 Invalid Sequence

* **Design Purpose：** The case where none are Dijkstra Sequences <br>

* **Test Cases：** <br>

```c

4 4

1 2 1

1 3 3

2 4 1

3 4 1

2

1 3 2 4

1 4 2 3

```

* **Results：** <br>

```c

No

No

```

* **Status**：passed <br>

### 3.7 Multi-Source Validation

* **Design Purpose：** The cases of Dijkstra Sequences with multiple different sources<br>

* **Test Cases：** <br>

```c

3 3

1 2 2

1 3 3

2 3 1

2

2 1 3

3 2 1

```

* **Results：** <br>

```c

No

Yes

```

* **Status**：passed <br>

## 4 Analysis and Comments

### 4.1 Time Complexity Analysis

#### 4.1.1  Graph Construction (`BuildGraph`)

* Each edge is inserted into two adjacency lists (for an undirected graph). Inserting a node at the head of a linked list takes **O(1)** time per insertion.

* For `ne` edges, total time: **O(ne)**.

#### 4.1.2 Heap Insertion (`InsertHeap`)

* Inserting a node into the heap takes **O(log n)** time due to the percolate-up operation (heapify up), where `n` is the number of elements in the heap.

* In the worst case, each insertion requires traversing the height of the heap, which is **log n** for a heap with `n` elements.

#### 4.1.3  Select Minimum Element (`selectmin`)

* **Searching for the minimum unvisited node**: The loop iterates through all `n` elements in the heap to find the minimum, taking **O(n)** time. This is inefficient for a heap, as the minimum should be at the root in a min-heap, but the code does not leverage this (it checks for unvisited nodes by scanning the entire heap).

* **Heapify-down after deletion**: Takes **O(log n)** time, as it traverses the height of the heap.

* **Total per call**: **O(n + log n) = O(n)**.

#### 4.1.4 Judge Function (`judge`)

- **Main Loop**: Runs for `nv` vertices (the sequence length).

* **Adjacency List Traversal**: For each vertex `s[i]`, traverse its adjacency list. Let `E` be the total number of edges in the graph. In the worst case, each edge is processed twice (once for each direction in an undirected graph), so total edge traversal time is **O(E)**.

* **Heap Insertions**: Each edge can trigger a heap insertion, leading to **O(E log nv)** time (since each insertion is **O(log nv)**).

* **`selectmin` Calls**: Each call to `selectmin` takes **O(nv)** time (due to the linear scan for unvisited nodes), leading to **O(nv × nv) = O(nv²)** time.

* **Total for `judge`**: **O(nv² + E log nv + E)**.

* For a dense graph where `E = O(nv²)`, this simplifies to **O(nv² log nv)**.

#### 4.1.5  Main Function

- **Reading Input and Building the Graph**: **O(ne)** for edge insertion.

- **Processing `num` Sequences**: Each sequence runs the `judge` function, so total time is **num × O(nv² log nv)**.

#### 4.1.6 Overall Time Complexity

- **Worst Case**: **O(ne + num × nv² log nv)**.

- The `selectmin` function’s linear scan for unvisited nodes is a critical inefficiency, as it negates the heap’s expected **O(log n)** time for minimum selection. This leads to a worse time complexity than standard Dijkstra’s algorithm (which uses a priority queue for **O((V+E) log V)** time).

### 4.2 Space Complexity Analysis

####  4.2.1 Graph Storage

- **Adjacency Lists**: For an undirected graph with `ne` edges, each edge is stored twice (once for each vertex), leading to **O(ne)** space for the adjacency list nodes.

- **Graph Structure (`Gnode`)**: Stores `MAXVERTEX` pointers to adjacency lists, taking **O(MAXVERTEX)** space.

- Total: **O(MAXVERTEX + ne)**.

#### 4.2.2 Heap Storage

- The heap stores up to `nv` nodes (each with a `vertex` and `weight`), taking **O(nv)** space for the heap array.

- Each heap node is a dynamically allocated pointer, but the array itself is fixed-size (`MAXVERTEX`), so space is **O(MAXVERTEX)**.

#### 4.2.3 Auxiliary Arrays in `judge`

- **`list` Array**: A 2D array of size `(nv+1) × 2`, taking **O(nv)** space.

- **Heap Structure**: **O(MAXVERTEX)** space for the heap array.

#### 4.2.4 Main Function

- Stores `num` sequences, each of size `nv+1`, taking **O(num × nv)** space.

#### 4.2.5 Overall Space Complexity

- **Worst Case**: **O(MAXVERTEX + ne + num × nv)**.

- The space complexity is dominated by the graph’s adjacency lists and the preallocated arrays/heap structures.

### 4.3 Possible further improvement measures

#### 4.3.1 Inefficiency of `selectmin`

**Issue**:

- The function performs a linear scan of the heap to find unvisited vertices, failing to leverage the min-heap property (the root node is the minimum value).

**Optimization**:

- Maintain an additional array to track whether vertices are in the heap, or mark visited vertices as invalid within the heap.

- Use a priority queue (e.g., C++'s `priority_queue`) that allows deleting arbitrary elements (combined with a union-find or marking mechanism).

#### 4.3.2 Duplicate Vertices in the Heap

**Issue**:

- The same vertex may be inserted into the heap multiple times due to distance updates, leading to redundant entries.

**Optimization**:

- Skip insertion if the new distance is greater than the recorded shortest distance.

- Retain all entries in the heap but determine whether to process them by comparing with the shortest distance in the `list` array.

## Appendix: Source Code (in C)

Executable code can be generated through Dev-C++.

```c

#include<stdio.h>

#include<stdlib.h>

#define MAXWEIGHT 200

#define MAXVERTEX 1000

struct AdjVnode // node structure

{

int vertex;

int weight;

struct AdjVnode *next;

};

typedef struct Vnode //The structure of adjacent nodes of a node.

{

struct AdjVnode *firstedge;

} *AdjList[MAXVERTEX];

struct Gnode //the structure of the graph

{

int nv;

int ne;

AdjList G;

};

void BuildGraph(struct Gnode *Graph,int r,int c,int w) //the function of building the graph

{

struct AdjVnode *s=(struct AdjVnode *)malloc(sizeof(struct AdjVnode)); //r->c

s->vertex=c;

s->weight=w;

s->next=Graph->G[r]->firstedge;

Graph->G[r]->firstedge=s;

struct AdjVnode *t=(struct AdjVnode *)malloc(sizeof(struct AdjVnode)); //c->r

t->vertex=r;

t->weight=w;

t->next=Graph->G[c]->firstedge;

Graph->G[c]->firstedge=t;

}

struct heapnode //structure of the heap node

{

int vertex;

int weight;

};

struct heap //the structure of the heap

{

struct heapnode *T[MAXVERTEX];

int size;

int cap;

};

struct heap *InsertHeap(int vertex,struct heap *H,int weight) //insert vertex in heap

{

H->size++;

H->T[H->size-1]=(struct heapnode *)malloc(sizeof(struct heapnode));

H->T[H->size-1]->vertex=vertex;

H->T[H->size-1]->weight=weight;

int s=H->size-1,i;

for(i=s;i>0&&H->T[(i-1)/2]->weight>weight;i=(i-1)/2)

{

H->T[i]->weight=H->T[(i-1)/2]->weight;

H->T[i]->vertex=H->T[(i-1)/2]->vertex;

}

H->T[i]->weight=weight;

H->T[i]->vertex=vertex;

return H;

}

int selectmin(struct heap *H,int **list) //select min in the heap

{

if(H->size==0) return MAXWEIGHT; //case1:empty heap

int min=MAXWEIGHT;

int adr=-1;

for(int i=0;i<H->size;i++)

{

if(list[H->T[i]->vertex][1]==0&&H->T[i]->weight<min)

{

min=H->T[i]->weight;

adr=i;

}

}

if(adr==-1) return MAXWEIGHT; //case2:all MAXWEIGHT

struct heapnode *temp=H->T[adr]; //case3:select min update the heap

H->T[adr]=H->T[H->size-1];

H->size--;

int i=adr;

while(2*i+1<H->size)

{

int left=2*i+1,right=2*i+2;

int smallest=left;

if(right<H->size&&H->T[right]->weight<H->T[left]->weight)

smallest=right;

if(H->T[i]->weight<=H->T[smallest]->weight) break;

struct heapnode *tempt=H->T[i];

H->T[i]=H->T[smallest];

H->T[smallest]=tempt;

i=smallest;

}

return min;

}

int judge(struct Gnode *Graph,int *s,int nv) //determine wheather the sequence is a Dijkstra sequence

{

int i;

int **list=(int **)malloc(sizeof(int *)*(nv+1)); //list:from 1 to nv

for(i=0;i<nv+1;i++)

{

list[i]=(int *)malloc(sizeof(int)*2); //Build a two-dimensional array to record the path

}

for(i=0;i<nv+1;i++)

{

list[i][0]=MAXWEIGHT;

list[i][1]=0; //Initialize the array

}

struct heap *H=(struct heap *)malloc(sizeof(struct heap));

H->size=0;

H->cap=nv;

H=InsertHeap(s[1],H,0);

int pm=MAXWEIGHT;

for(i=1;i<nv+1;i++)

{

if(i==1)

{

list[s[i]][0]=0;

list[s[i]][1]=1;

}

else if(list[s[i]][0]>pm) //Determine whether this vertice is the one with the shortest path

{

for(int j=0;j<nv+1;j++) free(list[j]);

free(list); return 0;

}

struct AdjVnode *g=Graph->G[s[i]]->firstedge;

while(g)

{

if(list[s[i]][0]+g->weight<list[g->vertex][0]&&list[g->vertex][1]==0)

{

list[g->vertex][0]=list[s[i]][0]+g->weight;

H=InsertHeap(g->vertex,H,list[g->vertex][0]);

}

g=g->next;

}

pm=selectmin(H,list); //select the mininum in the heap

list[s[i]][1]=1; //record this vertice has been used

}

for(i=0;i<nv+1;i++) free(list[i]);

free(list); //free memory

return 1;

}

int main()

{

int nv,ne,i;

scanf("%d %d",&nv,&ne); //input

struct Gnode *Graph=(struct Gnode *)malloc(sizeof(struct Gnode)); //initialize the graph structure

for (i=0;i<MAXVERTEX;i++)

{

Graph->G[i]=(struct Vnode *)malloc(sizeof(struct Vnode));

Graph->G[i]->firstedge=NULL;

}

Graph->nv=nv;

Graph->ne=ne;

for(i=0;i<ne;i++)

{

int r,c,w;

scanf("%d %d %d",&r,&c,&w);

BuildGraph(Graph,r,c,w); //input the graph

}

int num;

scanf("%d",&num);

for(i=0;i<num;i++)

{

int *s=(int *)malloc(sizeof(int)*(nv+1));

for(int j=1;j<nv+1;j++) //s: from 1 to nv

{

scanf("%d",&s[j]); //input the sequence

}

if(judge(Graph,s,nv)==1) printf("Yes\n"); //output

else printf("No\n");

free(s);

}

return 0;

}

```

## Declaration

*I hereby declare that all the work done in this project is of my independent effort.*