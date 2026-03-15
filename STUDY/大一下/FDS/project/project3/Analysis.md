### Time Complexity Analysis

#### `main` function

- **Input processing stage**:
    - Reading `nv` and `ne` is a constant - time operation, with a complexity of \(O(1)\).
    - Initializing the adjacency matrix `weight`, which is a two - dimensional array of size `(nv + 1) × (nv + 1)`, requires iterating through \((nv + 1)^2\) elements. The time complexity is \(O(nv^2)\).
    - Reading the information of `ne` edges. Each read operation takes constant time, so the complexity is \(O(ne)\).
    - Reading the number of query sequences `num` is a constant - time operation, with a complexity of \(O(1)\).
- **Sequence judgment stage**:
    - The loop runs `num` times, and each time the `judge` function is called. Since the time complexity of the `judge` function is \(O(nv^2)\), the complexity of this part is \(O(num\times nv^2)\).
- **Memory release stage**:
    - To release the memory of the `weight` array, it is necessary to iterate through `nv + 1` rows. The time complexity is \(O(nv)\).

#### `judge` function

- **Initialization stage**:
    - Memory is allocated for the two - dimensional `list` array and it is initialized. The `list` has `nv + 1` rows and 2 columns per row. The initialization operation needs to iterate through `nv + 1` rows, so the time complexity is \(O(nv)\).
- **Sequence traversal stage**:
    - The outer loop iterates through `nv` vertices in the sequence. For each vertex, there are two inner loops:
        - The first inner loop iterates through `nv` vertices to check if the shortest paths of the nodes connected to the current vertex need to be updated. The time complexity of this loop is \(O(nv)\).
        - The second inner loop also iterates through `nv` vertices to find the minimum path among the unvisited vertices. Its time complexity is \(O(nv)\).
    - The outer loop executes `nv` times, and the total time complexity of the inner loops each time is \(O(nv)\). So, the overall time complexity of this part is \(O(nv^2)\).
- **Memory release stage**:
    - To release the memory of the `list` array, it is necessary to iterate through `nv + 1` rows. The time complexity is \(O(nv)\).

  

Overall, the time complexity of the `judge` function is dominated by the sequence traversal stage, which is \(O(nv^2)\).


  

The time complexity of the `main` function is mainly determined by the sequence judgment stage, which is \(O(num\times nv^2)\).

#### Overall program

The time complexity of the entire program is dominated by the sequence judgment part in the `main` function. The final time complexity is \(O(num\times nv^2)\).

### Space Complexity Analysis

#### `main` function

- The main space overhead lies in the adjacency matrix `weight`, which is a two - dimensional array of size `(nv + 1) × (nv + 1)`. Therefore, the space complexity is \(O(nv^2)\).
- The `s` array created in each loop has a size of `nv + 1`. However, since it is reused inside the loop and the memory is released at the end of each loop, there is no cumulative additional space overhead.

#### `judge` function

- The main space overhead in the `judge` function is the two - dimensional `list` array. The `list` has `nv + 1` rows and 2 columns per row, so the space complexity is \(O(nv)\).


#### Overall program

The space complexity of the entire program is determined by the adjacency matrix `weight` in the `main` function, which is \(O(nv^2)\).

  

In summary, the time complexity of this program is \(O(num\times nv^2)\), and the space complexity is \(O(nv^2)\).