#pragma once
/* ============================================================================== =
*
*MIT License
*
*Copyright(c) 2025 Lev Zlotin
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
*The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* ============================================================================== =*/

#include <string>
#include "ExprToRpn.h"

#ifdef RPN_EXPORTS
#define RPN_API __declspec(dllexport)
#else
#define RPN_API __declspec(dllimport)
#endif

// RpnCalculator provides an interface for evaluating mathematical expressions using Reverse Polish Notation (RPN).
// It supports adding custom functions and operators, as well as standard mathematical operations.
//
// Usage:
//   - Add custom or standard functions/operators.
//   - Evaluate expressions in infix or RPN format.
//
// Members:
//   calc: Pointer to the internal FunctionShuntingYard engine.
//   verbose: Controls verbosity of output (e.g., prints RPN steps if enabled).
class RPN_API RpnCalculator
{
private:
    // Pointer to the internal shunting yard calculator engine.
    FunctionShuntingYard *calc;
    // Verbosity flag for debugging or detailed output.
    int verbose = 0;

public:
    // Constructs an RpnCalculator instance.
    // _verbose: Set to nonzero for verbose output.
    RpnCalculator(int _verbose = 0);

    // Destructor for RpnCalculator.
    virtual ~RpnCalculator();

    // Registers a custom function with the calculator.
    // name: The function name.
    // functionUniquePtr: Unique pointer to the function implementation.
    void addFunction(std::string const& name, function_ptr_t functionUniquePtr);

    // Registers a custom operator with the calculator.
    // name: The operator symbol.
    // operatorUniquePtr: Unique pointer to the operator implementation.
    void addOperator(std::string const& name, operator_ptr_t operatorUniquePtr);

    // Adds a set of standard mathematical functions (e.g., sin, cos, log).
    void addStandardFunctions();

    // Adds a set of standard mathematical operators (e.g., +, -, *, /).
    void addStandardOperators();

    // Evaluates an expression in RPN format.
    // input_rpn: The RPN expression as a string.
    // Returns: The result as a string.
    std::string calculate_rpn(const std::string & input_rpn) const;

    // Evaluates an expression in infix format (converts to RPN internally).
    // input: The infix expression as a string.
    // Returns: The result as a string.
    std::string calculate(const std::string & input) const; 

    // creates list of supported functions
    void enumerateFunctions(bool (*scan_func)(std::string const& name, IFunctionInfo const *)) const;


};

