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
#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <cmath>
#include <algorithm>
#include <memory>
#include <cctype>
#include <functional>
#include <stdexcept>

#ifdef RPN_EXPORTS
#define RPN_API __declspec(dllexport)
#else
#define RPN_API __declspec(dllimport)
#endif

// Interface representing operator information for the FunctionShuntingYard class.
// This interface defines the contract for operators, including their precedence,
// associativity, number of parameters, and calculation logic.
class IOperatorInfo
{
public:
    virtual ~IOperatorInfo() = default;
    // Returns the precedence of the operator.
    // Higher values indicate higher precedence.
    virtual int precedence() = 0;

    // Returns whether the operator is right-associative.
    // For example, the exponentiation operator (^) is typically right-associative.
    virtual bool isRightAssociative() = 0;

    // Returns the number of parameters the operator takes.
    // For example, binary operators like + and - take two parameters.
    virtual int num_parameters() = 0;

    // Performs the calculation for the operator given its arguments.
    // Args:
    // - args: A vector of strings representing the arguments for the operator.
    // Returns:
    // - A string representing the result of the calculation.
    virtual std::string calculate(const std::vector<std::string>& args) = 0;
};


// Interface representing function information for the FunctionShuntingYard class.
// This interface defines the contract for functions, including their arity
// (number of parameters) and calculation logic.
class IFunctionInfo
{
public:
    // Returns the number of parameters the function takes.
    // For example, a function like sin(x) takes one parameter, while max(a, b) takes two. For functions with variable number of parameters, returns -1
    virtual int num_parameters() = 0;

    // Performs the calculation for the function given its arguments.
    // Args:
    // - args: A vector of strings representing the arguments for the function. For function with variable number of parameters, 
    //         the last parameter should be the number of parameters
    // Returns:
    // - A string representing the result of the calculation.
    virtual std::string calculate(const std::vector<std::string>& args) = 0;
};
typedef std::unique_ptr<IOperatorInfo> operator_ptr_t;
typedef std::unique_ptr<IFunctionInfo> function_ptr_t;


// Class implementing the Shunting Yard algorithm for parsing mathematical expressions.
// FunctionShuntingYard supports custom operators and functions, allowing conversion
// from infix notation to Reverse Polish Notation (RPN) and evaluation of RPN expressions.
//
// Features:
// - Register custom operators and functions with user-defined precedence, associativity, and logic.
// - Tokenize infix expressions into components (numbers, operators, functions, brackets, etc.).
// - Convert infix expressions to RPN using the Shunting Yard algorithm.
// - Evaluate RPN expressions using registered operators and functions.
// - Print and list available functions.
class FunctionShuntingYard {
private:
    // Map of operator names to their operator information (precedence, associativity, etc.).
    std::unordered_map<std::string, operator_ptr_t> operators;
    // Map of function names to their function information (arity, calculation logic, etc.).
    std::unordered_map<std::string, function_ptr_t> functions;
    // Set of right-associative operator symbols (e.g., "^").
    //std::unordered_set<std::string> rightAssociative = { "^" };


    // helpers
    static bool isLeftBracket(const std::string& s) {
        return s == "(" || s == "[" || s == "{";
    }
    static bool isRightBracket(const std::string& s) {
        return s == ")" || s == "]" || s == "}";
    }
    static std::string matchingOpening(const std::string& closing) {
        if (closing == ")") return "(";
        if (closing == "]") return "[";
        if (closing == "}") return "{";
        return "";
    }
    bool isString(const std::string& token) const {
        if (token[0] == '"' && token[token.length() - 1] == '"')
            return true;
        return false;
    }

    bool isOperator(const std::string& token) const {
        return operators.find(token) != operators.end();
    }
    bool isFunction(const std::string& token) const {
        return functions.find(token) != functions.end();
    }
    int getPrecedence(const std::string& op) const {
        auto it = operators.find(op);
        return (it != operators.end()) ? it->second.get()->precedence() : 0;
    }
    bool isRightAssociative(const std::string& op) const {
        auto it = operators.find(op);
        return (it != operators.end()) ? it->second.get()->isRightAssociative() : false;
    }
    int getFunctionArity(const std::string& func) const {
        auto it = functions.find(func);
        return (it != functions.end()) ? it->second.get()->num_parameters() : 0;
    }


public:
    FunctionShuntingYard() = default;
    virtual ~FunctionShuntingYard()
    {
        operators.clear();
        functions.clear();

    }

    // Registers a new operator with the given name and operator information.
    void add_operator(std::string name, operator_ptr_t operator_info)
    {
        operators[name] = std::move(operator_info);
    }

    // Registers a new function with the given name and function information.
    void add_function(std::string name, function_ptr_t function_info)
    {
        functions[name] = std::move(function_info);
    }

    // Tokenizes an infix expression string into a vector of tokens.
    std::vector<std::string> tokenize(const std::string& expression, bool rpn_input);
    // Converts an infix expression string to a vector of RPN tokens.
    std::vector<std::string> infixToRPN(const std::string& infix);
    // Evaluates an RPN expression and returns the result as a string.
    std::string evaluateRPN(const std::vector<std::string>& rpn) const;
    // Prints the RPN expression to the standard output.
    void printRPN(const std::vector<std::string>& rpn) const;
    // Lists all registered functions to the standard output.
    void listFunctions() const;
};
