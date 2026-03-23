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