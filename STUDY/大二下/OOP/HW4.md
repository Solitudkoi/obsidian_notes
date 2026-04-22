9-1 AutoDiff
## Objective

Welcome to this assignment! You will be working with a modern C++ library that implements reverse-mode automatic differentiation (AutoDiff). This module is designed to build computational graphs, evaluate mathematical functions, and automatically compute exact gradients (derivatives) without relying on numerical approximations. This assignment will test your understanding of:

- **Object-Oriented Programming:** Utilizing polymorphism, virtual functions, and inheritance to build a computational graph.
- **Memory Management:** Designing a "Memory Arena" using `std::vector` and raw pointers to manage the lifecycle of dynamically allocated objects safely.
- **Operator Overloading:** Using the "Hidden Friends" idiom to create a natural, math-like syntax for C++ objects.

## The Skeleton Code

Your task is to _implement the core mathematical and graph traversal logic_ of this framework. You will be provided with a skeleton of the library split into three units (`ad_nodes.h`, `ad_graph.h`/`.cpp`, and `ad_var.h`/`.cpp`), where several key implementations have been removed and marked for you to complete.

Before writing any code, review the core components of the `ad` namespace:

- **`detail::Node`**: The polymorphic base struct for all operations. It requires a virtual destructor and pure virtual `forward()` and `backward()` methods, alongside data members for `value` and `adjoint` (the gradient).
- **Math Nodes (e.g., `AddNode`, `MulNode`, `SinNode`)**: Derived structs representing specific mathematical operations. They compute their own values and distribute their adjoints to their operands using the Chain Rule.
- **`Graph`**: The _engine_ and _memory arena_. It owns the raw pointers to all nodes, executes the forward/backward passes, and cleans up memory in its destructor.
- **`Var`**: The public-facing handle. It wraps a `Graph*` and a `NodeId`, providing overloaded operators (like `+`, `*`, `sin`) so users can build math functions naturally.

The specific tasks include:

- **In `ad_nodes.h`**:
    - Define the virtual destructor and declare the pure virtual `forward()`/`backward()` methods in the base `Node` struct.
    - Implement the struct definitions for `MulNode`, `DivNode`, `SinNode`, and `CosNode`. Remember that `backward()` applies the Chain Rule by accumulating the current `adjoint` into the operands' `adjoint`s.
- **In `ad_graph.cpp`**:
    - Implement the node creation helpers: `mul`, `div`, `sin`, and `cos`.
    - Implement the `forward()` method. You need to iterate over the `nodes_` vector from start to finish, calling `forward()` on each node.
    - Implement the `backward(NodeId target)` method. You must first reset all node adjoints to 0.0, set the target node's adjoint to 1.0, and then iterate through the vector in _reverse_ order, calling `backward()` on each node.
    - Implement the `clear()` method to safely `delete` all dynamically allocated nodes and clear the vector.
- **In `ad_var.h` & `ad_var.cpp`**:
    - Implement the missing overloaded math operators (`*`, `/`, `sin`, `cos`) for the `Var` class. Be sure to handle both `Var`-to-`Var` and `Var`-to-`double` overloads.
    - Implement the `backward()` wrapper method in `ad_var.cpp`.

## Test Sample

The `main.cpp` includes test cases like this to test a multi-variable math function:

▾

nano_test::Register test_nd_to_1d("nD_to_1D", []() {

ad::Graph g;

ad::Var x = ad::Var::Input(g, 2.0);

ad::Var y = ad::Var::Input(g, 4.0);

  

ad::Var f = pow2(x) + (x * y) + sqrt(y);

g.forward();

f.backward();

  

double expected_f = 14.0;

double expected_df_dx = 8.0;

double expected_df_dy = 2.25;

  

nano_test::expect_approx_eq(f.value(), expected_f);

nano_test::expect_approx_eq(x.adjoint(), expected_df_dx);

nano_test::expect_approx_eq(y.adjoint(), expected_df_dy);

});

## Optional Tasks (not included in the score)

Visualizing the computational graph is an excellent learning tool. Extend the `detail::Node` base struct with virtual methods like `virtual std::string op_name() const = 0;` and `virtual std::vector<Node*> get_operands() const;`. Then, add a `print_dot(std::ostream& os)` method to the `Graph` class that iterates through the nodes and prints them in the Graphviz DOT format. You can copy the resulting output from your console into a tool like [Graphviz Online](https://dreampuf.github.io/GraphvizOnline/) to see a flowchart of your mathematical functions.

## Design Discussion

For those interested in software architecture and the darker corners of C++, consider these _facts_ about our AutoDiff framework's design:

**1. Deferred Execution vs. Eager Execution**  
Our library uses _Deferred Execution_ ("Define-and-Run"). You build the math, and then explicitly call `g.forward()` to evaluate it. Commercial frameworks like PyTorch use _Eager Execution_ ("Define-by-Run"), where the math is evaluated instantly the moment you type `x + y`. What are the performance advantages of our deferred approach when writing an iterative optimization loop (like Gradient Descent)? Why did we include a `set_value()` method?

**2. The Memory Arena and Smart Pointers**  
Look closely at the `Graph` class. It manages an `std::vector<detail::Node*>` using raw pointers, explicitly calling `delete` in the destructor. In modern C++, the use of raw, _owning_ pointers is generally discouraged in favor of smart pointers like `std::shared_ptr` or `std::unique_ptr`. If we were to refactor `Graph` to use `std::unique_ptr<detail::Node>`, how would that change the implementation of our `push_node` helper and the `clear` method?

**3. The "Hidden Friend" Idiom**  
In `ad_var.h`, math operators are implemented _inline_ as `friend` functions. This modern C++ technique creates non-member functions hidden from standard name lookup. They are only discoverable via Argument-Dependent Lookup (ADL) when a `Var` object is passed. Why is this preferable to standard non-member overloads? Consider how avoiding namespace pollution—like hiding our custom `sin` from standard `double` calculations—speeds up compilation and produces cleaner error messages.

时间限制

10000 ms

内存限制

64 MB

CPU核数限制

1