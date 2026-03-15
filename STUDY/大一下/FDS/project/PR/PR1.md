# **Algorithm Verification: Dijkstra's Shortest Path Validation**

  

## Chapter 1: Introduction

**Problem Description**

Dijkstra's algorithm is a well-known method for finding the shortest paths from a single source vertex to all other vertices in a graph with non-negative edge weights. Given a graph and a sequence of vertices with their claimed shortest distances, this project aims to verify whether the sequence is a valid output of Dijkstra's algorithm.  

**Background**

- Dijkstra's Algorithm:
    
    Maintains a priority queue to greedily select the next closest vertex. Relaxes edges to update the shortest path estimates. Terminates when all vertices are processed.  
    
- Verification Challenge:
    

The input sequence must follow the order in which Dijkstra processes vertices.

All distances must satisfy the optimal substructure property (i.e., no shorter path exists).

  

## Chapter 2: Algorithm Specification

### 1. Data Structures

- I use the Adjacency List (which is defined as `VT* V`) to store the Graph.
    
    Adjacency lists are generally more efficient than adjacency matrices for graph storage due to their superior space and time complexity in most practical scenarios. While adjacency matrices require space regardless of edge density, adjacency lists use only space by storing only existing edges, making them ideal for sparse graphs like social networks or web page linkages. Traversal operations such as DFS or BFS benefit significantly from adjacency lists, as they allow neighbor retrieval in time per edge, leading to complexity compared to the time required by matrices. Additionally, adjacency lists handle dynamic graph modifications more gracefully, supporting edge insertions in and vertex additions in time, whereas matrices often demand costly resizing. Although adjacency matrices excel in dense graphs or applications requiring constant-time edge checks (e.g., network flow analysis), adjacency lists remain the preferred choice for most algorithms (e.g., Dijkstra’s, Prim’s) and large-scale systems due to their scalability and memory efficiency.
    

  

- Auxiliary Arrays
    
    In this code, the `visited` and `dist` arrays serve crucial roles in implementing and verifying Dijkstra's algorithm. The `visited` array is a boolean array that tracks which vertices have been permanently processed (i.e., their shortest paths from the source have been confirmed). This ensures each vertex is processed exactly once, maintaining Dijkstra's greedy property. The `dist` array stores the current shortest known distances from the source vertex to all other vertices. During execution, it is dynamically updated through edge relaxation: whenever a shorter path to a vertex is found via a processed neighbor, its distance is updated in `dist`. Together, these arrays enforce Dijkstra's core logic—`visited` guarantees vertices are handled in order of increasing distance, while `dist` propagates optimal substructure by iteratively improving path estimates until all shortest paths are determined. Their combined use validates whether the input sequence could legitimately result from Dijkstra's algorithm.  
    

  

### 2.Algorithm description

- Pseudocode

C

```C
function isDijkstra(d, V, Nv):

    dist = [∞] * Nv       // Initialize distances

    visited = [False] * Nv // Track processed vertices

    dist[d[0]] = 0        // Source distance is 0



    for i from 0 to Nv-1:

        u = d[i]           // Current vertex to process



        # Check if u has the smallest dist among unvisited vertices

        for v from 0 to Nv-1:

            if not visited[v] and dist[v] < dist[u]:

                return False  // Invalid sequence



        visited[u] = True  // Mark u as processed



        # Relax all edges from u

        if dist[u] ≠ ∞:

            for each neighbor v of u in adjacency list:

                if not visited[v] and dist[u] + weight(u,v) < dist[v]:

                    dist[v] = dist[u] + weight(u,v)  // Update distance



    return True  // Valid Dijkstra output
```

### 3. Key Steps

1. Initialization: Set all distances to infinity except the source.
    
2. Greedy Selection Check: Ensure the input sequence follows Dijkstra’s order.
    
3. Edge Relaxation: Update distances if a shorter path is found.
    
4. Validation: If any check fails, the sequence is invalid.  
    

## Chapter 3: Testing Results

| |  
|  
| |

|Test Case|Description|Expected Result|Actual Result|Status|
|---|---|---|---|---|
|1|Valid Dijkstra sequence|Yes|Yes|Pass|
|2|Invalid order (smaller dist skipped)|No|No|Pass|
|3|Disconnected graph (unreachable vertex)|Yes|Yes|Pass|

Test cases with actual numbers:  
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

  

and their results are:

Yes

Yes

Yes

No

  

### Chapter 4: Analysis and Comments

### 1. Time Complexity

- Worst Case: O(Nv²+Ne)

O(Nv²)for checking vertex order.

O(Ne) for edge relaxation.

- Best Case: O(Nv²) (if early termination on failure).  
    

### 2. Space Complexity

- Graph Storage:O(Nv+Ne) (adjacency list).
    
- Auxiliary Arrays: O(Nv)
    

  

## Appendix: Source Code (C)

C

  

```C
#include<stdio.h>   // Standard Input/Output library
#include<stdlib.h>  // Standard Library for memory allocation, etc.
#include<limits.h>  // Contains constants like INT_MAX

// Vertex structure definition
typedef struct Vertex
{
    int VerNum;     // Stores the connected vertex number
    int weight;     // Stores the edge weight to connected vertex
    struct Vertex* Next;  // Pointer to next adjacent vertex
} VT;

typedef VT* Graph;  // Graph type defined as pointer to Vertex

/*
if given distance array is a valid Dijkstra result
d Array of vertex distances to verify
V Graph represented as adjacency list
 Nv Number of vertices
return true if valid Dijkstra result, false otherwise
 */
bool isdijkstra(int d[], VT* V, int Nv)
{
    // Allocate distance array (stores shortest path from source)
    int* dist = (int*)malloc(Nv * sizeof(int));
    // Allocate visited array (tracks processed vertices)
    bool* visited = (bool*)malloc(Nv * sizeof(bool));

    // Initialize arrays
    for (int i = 0; i < Nv; i++) {
        dist[i] = INT_MAX;   // Set initial distances to infinity
        visited[i] = false;  // Mark all vertices as unprocessed
    }
    dist[d[0]] = 0;  // Distance from source to itself is 0

    // Process vertices in given order
    for (int i = 0; i < Nv; i++) {
        int u = d[i];  // Current vertex to process

        // Verify u has the minimum distance among unprocessed vertices
        for (int v = 0; v < Nv; v++) {
            if (!visited[v] && dist[v] < dist[u]) {
                free(dist);
                free(visited);
                return false;  // Found smaller distance - invalid sequence
            }
        }

        visited[u] = true;  // Mark current vertex as processed

        // Relaxation: Update distances of adjacent vertices
        VT* node = V[u].Next;
        if (dist[u] != INT_MAX) {  // Only if current distance is finite
            while (node != NULL) {
                int v = node->VerNum;
                int weight = node->weight;
                // Update if found shorter path
                if (!visited[v] && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                }
                node = node->Next;
            }
        }
    }

    // Clean up
    free(dist);
    free(visited);
    return true;  // Passed all checks
}

int main()
{
    int Nv, Ne;  // Number of vertices and edges
    scanf("%d%d", &Nv, &Ne);

    // Create adjacency list representation
    VT* V = (VT*)malloc(Nv * sizeof(VT));
    for (int i = 0; i < Nv; i++) {
        V[i].Next = NULL;  // Initialize all adjacency lists
    }

    // Read and store edges
    int ve1, ve2, weight;
    for (int i = 0; i < Ne; i++) {
        scanf("%d%d%d", &ve1, &ve2, &weight);
        if (weight < 0) {
            printf("Edge weight must be non-negative!\n");
            exit(1);
        }// Make sure that weight is nonnegative
        ve1--; ve2--;  // Convert to 0-based indexing

        // Create new edge node
        VT* newnode = (VT*)malloc(sizeof(VT));
        newnode->VerNum = ve2;
        newnode->weight = weight;
        // Insert at head of adjacency list
        newnode->Next = V[ve1].Next;
        V[ve1].Next = newnode;
    }

    // Process test cases
    int K;  // Number of test cases
    scanf("%d", &K);
    int* result = (int*)malloc(K * sizeof(int));

    for (int i = 0; i < K; i++) {
        int* d = (int*)malloc(Nv * sizeof(int));
        // Read distance sequence
        for (int j = 0; j < Nv; j++) {
            scanf("%d", &d[j]);
            d[j]--;  // Convert to 0-based
        }
        // Verify and store result
        result[i] = isdijkstra(d, V, Nv) ? 1 : 0;
        free(d);
    }

    // Output results
    for (int i = 0; i < K; i++) {
        printf(result[i] ? "Yes\n" : "No\n");
    }

    // Clean up memory
    free(result);
    // Free adjacency lists
    for (int i = 0; i < Nv; i++) {
        VT* current = V[i].Next;
        while (current != NULL) {
            VT* temp = current;
            current = current->Next;
            free(temp);
        }
    }
    free(V);

    return 0;
}
```

  
  

## Declaration

I hereby declare that all the work done in this project titled **Dijkstra's Algorithm Verification** is of my _independent_ effort.