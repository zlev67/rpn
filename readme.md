RPN Calculator
Overview

This project provides:

RPN Calculator DLL
A reusable C++14 library for parsing and evaluating mathematical expressions using Reverse Polish Notation (RPN).
The DLL is designed for extensibility, allowing users to implement custom functions and operators.
It can also convert infix notation to RPN and calculate the result of the expression.

MoreFunctions
Examples of complex function implementations such as retrieving stock data by name or currency conversion.

RPN Calculator Executable
A standalone graphical calculator application that leverages the DLL for infix and RPN expression evaluation.

Command-Line Calculator (c.exe)
A lightweight console tool for quick calculations directly from the command line.

✨ Features

Supports both infix and RPN (postfix) notation.

Can automatically convert infix expressions to RPN and evaluate them.

Built-in standard mathematical functions and operators, including:

Operators: +, -, *, /, %, **, |, &, ^, <<, >>

Functions: sin, cos, tan, asin, acos, atan, ln, lg, expn, sqrt, abs, floor, ceil, log, pow, avg, g2r, r2g, hex, oct, bin, etc.

Easily extensible: users can add custom functions and operators by implementing simple interfaces.

🧩 RPN Calculator DLL
Usage

Include the DLL header in your project:

#include "RpnCalculator.h"


Link against the DLL and ensure the DLL is available at runtime.

Create and use the calculator:

RpnCalculator calc;
calc.addStandardFunctions();
calc.addStandardOperators();

std::string result = calc.calculate("2 + 2 * 2");         // Infix expression
std::string rpn_result = calc.calculate_rpn("2 2 2 * +"); // RPN expression


Extend with custom functions/operators:

Implement the IFunctionInfo or IOperatorInfo interface.

Register your implementation using addFunction or addOperator.

Example: Adding a Custom Function
class MyFunc : public IFunctionInfo {
public:
    int num_parameters() override { return 1; }
    std::string calculate(const std::vector<std::string>& args) override {
        // Custom logic here
        return args[0];
    }
};

// Registration example
calc.addFunction("myfunc", std::unique_ptr<MyFunc>(new MyFunc()));

🖥️ RPN Calculator Executable

The graphical executable provides a user-friendly interface for evaluating expressions in both infix and RPN notation.

Features

Input expressions in infix or RPN form.

View results instantly.

Switch between modes using radio buttons.

Clear error handling and informative messages.

Running

Simply build and run the executable. Enter your expression and select the desired mode.

You can bring up or close the application window at any time by pressing Ctrl + Shift + R.

⚙️ Command-Line Calculator (c.exe)

The c.exe tool is a lightweight command-line interface for the RPN Calculator library.
It enables quick evaluation of expressions directly from a terminal or script.

Usage
List all functions and operators
c -l

Calculate an RPN (postfix) expression
c -r "2 3 + 4 *"


Output:

20

Calculate an infix expression
c "2 + 3 * 4"


Output:

14

Notes

The tool automatically loads standard functions and operators from the RPN DLL.

Useful for:

Command-line calculations

Integration into automation scripts

Quick testing of expressions

Supports all features of the RPN Calculator, including custom functions and conversions.

🧮 Examples

Here are some practical command-line examples:

Basic Arithmetic
c "5 + (3 * 2) - 4 / 2"


Output:

9

Using Functions
c "pow(2, 3) + sqrt(16)"


Output:

12

Trigonometry
c "sin(0.5) + cos(0.5)"


Output:

1.357008

Conversion and Bitwise
c "hex(255)"
# Output: FF

c "bin(13)"
# Output: 1101

c "15 & 7"
# Output: 7

RPN Mode
c -r "2 3 4 * +"


Output:

14

Listing Available Functions
c -l


Sample output:

Available functions:
abs, avg, bin, ceil, cos, expn, floor, g2r, hex, lg, ln, log, oct, pow, r2g, sin, sqrt, tan, ...

🧠 Extending the Calculator

To add new functions or operators:

Implement the appropriate interface (IFunctionInfo or IOperatorInfo).

Register your implementation with the calculator instance using addFunction or addOperator.

🏗️ Building

Requires a C++14-compatible compiler.

Open the solution in Visual Studio and build:

The DLL

The GUI executable

The c.exe command-line tool

📄 License

MIT License
See LICENSE
 for details.