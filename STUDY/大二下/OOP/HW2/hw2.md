9-1 NanoTest
## Objective

Welcome to this assignment! You will be working with a modern, header-only C++ library that implements a lightweight unit-testing framework. This module is designed to register, execute, and report the results of software tests using a minimal footprint. This assignment will test your understanding of:

- **Basic Templates:** Writing template functions to compare generic types.
- **Modern C++ Features:** Utilizing C++20's `std::source_location` to capture file and line numbers without relying on preprocessor macros.
- **Design Patterns:** Implementing the _Singleton_ pattern to manage the global test registry securely.
- **Exception Handling:** Creating custom exception classes and using `try/catch` blocks to isolate failing tests without crashing the entire test suite.
- **Functional Programming:** Using `std::function` and lambda expressions to store and execute deferred test routines.
- **Container Manipulation:** Working with `std::vector` container.

## The Skeleton Code

Your task is to _implement the core testing logic_ of this framework. You will be provided with a skeleton of `nano_test.h`, where several key implementations have been removed and marked with `// [todo] ...`.

Before writing any code, review the core components of the `nano_test` namespace:

- **`TestFailure`** (inherits from `std::runtime_error`): A custom exception struct thrown whenever an assertion fails.
- **`expect_true`**: An assertion function. It evaluates a condition and throws a `TestFailure` with detailed file/line information if the condition is not met.
- **`expect_eq`**: A template assertion function. It evaluates whether two values are equivalent and throws a `TestFailure` with detailed file/line information if the equivalence is not met.
- **`Registry`**: The _manager_ class designed as a Singleton. It stores all registered test cases (as `std::function<void()>`) in a `std::vector` and handles the execution loop and summary reporting.
- **`Register` & `RegisterFixture`**: Helper structures that exploit static initialization and RAII to automatically register tests into the `Registry` before `main()` even begins.

The specific tasks include:

- Implement the `instance()` method in the `Registry` class. You must ensure only one instance of the registry ever exists.
- Implement the `add()` method in the `Registry` class. You need to construct a new test case and add it into the registry.
- Complete the `run()` method in the `Registry` class. You need to iterate over the `tests` vector, execute each function inside a `try/catch` block, catch any `std::exception`, and print the formatted console output.
- Implement the template function `expect_eq`. It needs to compare `actual` and `expected` parameters and construct a detailed error string using `std::source_location` if they do not match.

## Test Sample

The `main.cpp` might look something like this to test basic math and string operations:
```


#include "nano_test.h"

  

// Stateless test registration

nano_test::Register test1("MathTest.Addition", []() {

nano_test::expect_eq(2 + 2, 4);

nano_test::expect_true(1 + 4 > 3);

});

  

nano_test::Register test2("MathTest.IntentionalFailure", []() {

nano_test::expect_eq(10, 5); // This will throw and be caught by run()

});

  

int main() {

return nano_test::Registry::instance().run();

}

```

## Console Output

When you compile and run your tests, the terminal output should look like this. The serialization of the console output for the above sample:

[ RUN ] MathTest.Addition

[ OK ] MathTest.Addition

[ RUN ] MathTest.IntentionalFailure

[ FAILED ] MathTest.IntentionalFailure - Assertion failed at main.cpp:16

Expected: 5

Actual: 10

---------- Test Summary ----------

Total: 2, Passed: 1, Failed: 1

----------------------------------

## Optional Tasks (not included in the score)

**1. ANSI Terminal Colors**  
Professional testing frameworks highlight passing tests in green and failing tests in red. You can add this visual feature to your framework by using ANSI escape codes. Try defining a namespace `colors` with `constexpr` strings (e.g., `constexpr const char *RED = "\033[31m";` and `constexpr const char *RESET = "\033[0m";`) and applying them to your `std::cout` statements in the `Registry::run()` loop.

**2. Command-Line Filtering**  
Currently, the framework runs every test registered in the system. As test suites grow, developers often want to run only a subset of tests. Implement a command-line argument parser in `main.cpp` that accepts a filter string (e.g., `./run_tests --filter=MathTest`). Modify the `Registry::run()` method to accept this filter string and only execute tests whose names contain the filter substring.

## Design Discussion

For those interested in software architecture and the darker corners of C++, consider these _facts_ about our testing framework's design:

**1. The Singleton Pattern and Initialization**  
The `Registry` class relies on the Singleton pattern to store tests globally. In C++, the order in which global/static variables are initialized across different translation units (_.cpp_ files) is strictly undefined. This is known as the "Static Initialization Order Fiasco." How does the specific implementation of `Registry::instance()` (using a static local variable inside a static method) solve this problem? What would happen if the `std::vector<TestCase> tests` was simply a standard global variable rather than being encapsulated inside the Singleton?

**2. Exceptions vs. Error Codes**  
Consider the use of exceptions for test failures. What are the performance implications of throwing an exception versus returning an error code? Why is an exception preferable in the context of an assertion failing deep inside a helper function called by a test?

**3. Parameter Passing and Memory Efficiency**  
Look closely at the `Registry::add` method and the `Register` constructors. We pass the test name (`const std::string&`) and the test lambda (`const std::function<void()>&`) by `const` reference all the way down the call stack until they are finally stored in the `std::vector`. Why do we pass these objects by `const` reference instead of by value? If we passed them by value, how many unnecessary, expensive memory allocations and copies would be made before the data is safely stored in the vector? If you are familiar with modern C++11 move semantics (`std::move`), how would that feature change the way we write these constructors and the `add` method? Why is "pass-by-value and `std::move`" often considered the preferred modern C++ idiom for functions that take ownership of data?



# 9-1 NanoTest

## 目标

欢迎来到本次作业！你将使用一个现代的、仅头文件（header-only）的 C++ 库，它实现了一个轻量级的单元测试框架。该模块旨在以最小的开销来注册、执行并报告软件测试结果。本次作业将考察你对以下内容的理解：

* **基础模板（Templates）：** 编写模板函数以比较通用类型
* **现代 C++ 特性：** 使用 C++20 的 `std::source_location` 获取文件名和行号，而无需依赖预处理宏
* **设计模式：** 实现**单例模式（Singleton）**来安全地管理全局测试注册表
* **异常处理：** 创建自定义异常类，并使用 `try/catch` 块隔离失败的测试，避免整个测试套件崩溃
* **函数式编程：** 使用 `std::function` 和 lambda 表达式来存储和执行延迟的测试逻辑
* **容器操作：** 使用 `std::vector` 容器

---

## 提供的骨架代码

你的任务是**实现该框架的核心测试逻辑**。你会获得一个 `nano_test.h` 的骨架文件，其中若干关键实现被移除，并标记为 `// [todo] ...`。

在编写代码之前，请先了解 `nano_test` 命名空间中的核心组件：

* **`TestFailure`**（继承自 `std::runtime_error`）：当断言失败时抛出的自定义异常结构
* **`expect_true`**：断言函数。如果条件不成立，则抛出包含详细文件/行号信息的 `TestFailure`
* **`expect_eq`**：模板断言函数。用于判断两个值是否相等，不相等时抛出包含详细信息的 `TestFailure`
* **`Registry`**：作为**管理器（manager）**的类，采用单例模式。它使用 `std::vector` 存储所有注册的测试（类型为 `std::function<void()>`），并负责执行和输出测试结果
* **`Register` 和 `RegisterFixture`**：辅助结构，利用静态初始化和 RAII，在 `main()` 执行之前自动将测试注册到 `Registry`

---

## 具体任务

你需要完成以下实现：

* 实现 `Registry` 类中的 `instance()` 方法，确保全局只存在一个实例
* 实现 `Registry` 类中的 `add()` 方法，用于创建并注册新的测试用例
* 完成 `Registry` 类中的 `run()` 方法：

  * 遍历 `tests` 向量
  * 在 `try/catch` 块中执行每个测试
  * 捕获 `std::exception`
  * 输出格式化的测试结果
* 实现模板函数 `expect_eq`：

  * 比较 `actual` 和 `expected`
  * 如果不相等，使用 `std::source_location` 构造详细错误信息

---

## 测试示例

`main.cpp` 可能如下所示，用于测试基础数学和字符串操作：

```cpp
#include "nano_test.h"

// 无状态测试注册
nano_test::Register test1("MathTest.Addition", []() {
    nano_test::expect_eq(2 + 2, 4);
    nano_test::expect_true(1 + 4 > 3);
});

nano_test::Register test2("MathTest.IntentionalFailure", []() {
    nano_test::expect_eq(10, 5); // 这里会抛异常并被 run() 捕获
});

int main() {
    return nano_test::Registry::instance().run();
}
```

---

## 控制台输出

编译并运行后，终端输出应类似如下：

```
[ RUN ] MathTest.Addition
[ OK ] MathTest.Addition
[ RUN ] MathTest.IntentionalFailure
[ FAILED ] MathTest.IntentionalFailure - Assertion failed at main.cpp:16
Expected: 5
Actual: 10
---------- Test Summary ----------
Total: 2, Passed: 1, Failed: 1
----------------------------------
```

---

## 可选任务（不计分）

### 1. ANSI 终端颜色

专业测试框架通常用绿色显示通过的测试，用红色显示失败的测试。你可以使用 ANSI 转义码实现这一功能。

例如定义：

```cpp
namespace colors {
    constexpr const char *RED = "\033[31m";
    constexpr const char *RESET = "\033[0m";
}
```

并在 `Registry::run()` 的输出中应用这些颜色。

---

### 2. 命令行过滤

当前框架会运行所有测试。随着测试数量增加，开发者通常希望只运行部分测试。

你可以：

* 在 `main.cpp` 中实现命令行参数解析（如 `./run_tests --filter=MathTest`）
* 修改 `Registry::run()` 方法，使其只运行名称包含该字符串的测试

---

## 设计讨论

如果你对软件架构和 C++ 的一些“深水区”感兴趣，可以思考以下问题：

### 1. 单例模式与初始化顺序

`Registry` 使用单例模式来存储测试。在 C++ 中，不同编译单元（.cpp 文件）中全局/静态变量的初始化顺序是**未定义的**，这被称为：

👉 **“静态初始化顺序灾难”（Static Initialization Order Fiasco）**

思考：

* 为什么 `Registry::instance()` 使用“函数内静态变量”可以避免这个问题？
* 如果 `std::vector<TestCase> tests` 是一个普通全局变量，而不是封装在单例中，会发生什么？

---

### 2. 异常 vs 错误码

思考测试失败时使用异常的意义：

* 抛异常 vs 返回错误码，性能有什么区别？
* 为什么在深层函数中发生断言失败时，用异常更合理？

---

### 3. 参数传递与内存效率

观察 `Registry::add` 和 `Register` 构造函数：

* 为什么使用 `const std::string&` 和 `const std::function<void()>&`（按引用传递）？
* 如果改为按值传递，会产生多少不必要的拷贝和内存分配？

进一步思考：

* 如果使用 C++11 的移动语义（`std::move`），实现会如何改变？
* 为什么“按值传递 + std::move”是现代 C++ 中常见的推荐写法（尤其是需要接管所有权的场景）？
