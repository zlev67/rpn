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
#include <memory>
#include <unordered_map>


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


