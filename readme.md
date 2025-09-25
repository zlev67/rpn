# RPN Calculator

## Overview

This project provides:
- **RPN Calculator DLL**: A reusable C++14 library for parsing and evaluating mathematical expressions using Reverse Polish Notation (RPN). The DLL is designed for extensibility, allowing users to implement custom functions and operators. It is also capable of converting infix notation to RPN and calculating the result of the expression.
- **RPN Calculator Executable**: A standalone calculator application that leverages the DLL for RPN and infix expression evaluation.

---

## Features

- Supports both infix and RPN (postfix) notation.
- Can convert infix expressions to RPN automatically and evaluate them.
- Built-in standard mathematical functions and operators, including:
  - Arithmetic: `+`, `-`, `*`, `/`, `%`, `**`, `|`, `&`, `^`, `<<`, `>>`
  - Functions: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `ln`, `lg`, `expn`, `sqrt`, `abs`, `floor`, `ceil`, `log`, `pow`, `avg`, `g2r`, `r2g`, `hex`, `oct`, `bin`
- Easily extensible: users can add custom functions and operators by implementing simple interfaces.
- Exception-safe and memory-leak free (C++14).

---

## RPN Calculator DLL

### Usage

1. **Include the DLL header** in your project:
```cpp
#include "RpnCalculator.h"
```

2. **Link against the DLL** and ensure the DLL is available at runtime.

3. **Create and use the calculator:**
```cpp
RpnCalculator calc;
calc.addStandardFunctions();
calc.addStandardOperators();
std::string result = calc.calculate("2 + 2 * 2"); // Infix expression
std::string rpn_result = calc.calculate_rpn("2 2 2 * +"); // RPN expression
```

4. **Extend with custom functions/operators:**
    - Implement the `IFunctionInfo` or `IOperatorInfo` interface.
    - Register your implementation using `addFunction` or `addOperator`.

### Example: Adding a Custom Function

```cpp
class MyFunc : public IFunctionInfo {
public:
    int num_parameters() override { return 1; }
    std::string calculate(const std::vector<std::string>& args) override {
        // Custom logic here
        return args[0];
    }
};
calc.addFunction("myfunc", std::unique_ptr<MyFunc>(new MyFunc()));
```

---

## RPN Calculator Executable

The executable provides a user-friendly interface for evaluating expressions in both infix and RPN notation.

### Features

- Input expressions in infix or RPN.
- View results instantly.
- Switch between modes using radio buttons.
- Error handling and informative messages.

### Running

Simply build and run the executable. Enter your expression and select the desired mode.

You can bring up or close the application window at any time by pressing **Ctrl+Shift+R**.

---

## Extending the Calculator

To add new functions or operators:
1. Implement the appropriate interface (`IFunctionInfo` or `IOperatorInfo`).
2. Register your implementation with the calculator instance using `addFunction` or `addOperator`.

---

## Building

- Requires a C++14-compatible compiler.
- Open the solution in Visual Studio and build both the DLL and the executable.

---

## License

MIT License. See [LICENSE](LICENSE) for details.
