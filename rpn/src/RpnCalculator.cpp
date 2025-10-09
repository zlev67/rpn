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
#include <cmath>
#include <regex>
#include <bitset>
#include "RpnCalculator.h"
#include <windows.h>

#include <sstream>
#include <cstdlib>
#include "ExprToRpn.h"
#include "Numeric.h"


// Represents an arithmetic operator used in the RPN calculator.
// This class implements the IOperatorInfo interface and provides details about the operator,
// such as its precedence, associativity, and calculation logic.
class ArithmeticOperator : public IOperatorInfo
{
    // The type of the operator (e.g., +, -, *, /, etc.).
    std::string operator_type;

    // Indicates whether the operator is right-associative.
    bool m_isRightAssociative;

    // The precedence of the operator. Higher values indicate higher precedence.
    int m_precedence;

public:
    // Destructor for the ArithmeticOperator class.
    virtual ~ArithmeticOperator()
    {
    }

    // Constructor for the ArithmeticOperator class.
    // Args:
    // - op_type: The type of the operator (e.g., +, -, *, /, etc.).
    // - _verbose: Optional verbosity level (default is 0).
    ArithmeticOperator(std::string op_type, int _verbose = 0) : operator_type(op_type)
    {
        m_isRightAssociative = false;
        m_precedence = -1;

        // Set precedence and associativity based on the operator type.
        if (op_type == "|")
            m_precedence = 1;
        else if (op_type == "^")
            m_precedence = 2;
        else if (op_type == "&")
            m_precedence = 3;
        if (op_type == ">>" || op_type == "<<")
            m_precedence = 4;
        if (op_type == "+" || op_type == "-")
            m_precedence = 5;
        else if (op_type == "*" || op_type == "/" || op_type == "%")
            m_precedence = 6;
        else if (op_type == "**")
        {
            m_precedence = 7;
            m_isRightAssociative = true;
        }
    }

    // Returns the number of parameters the operator takes.
    // For arithmetic operators, this is always 2.
    virtual int num_parameters()
    {
        return 2;
    }

    // Returns whether the operator is right-associative.
    virtual bool isRightAssociative()
    {
        return m_isRightAssociative;
    }

    // Returns the precedence of the operator.
    virtual int precedence()
    {
        return m_precedence;
    }

    // Performs the calculation for the operator given its arguments.
    // Args:
    // - args: A vector of strings representing the arguments for the operator.
    // Returns:
    // - A string representing the result of the calculation.
    virtual std::string calculate(const std::vector<std::string>& args)
    {
        long double a = Numeric::str_to_ld(args[0]);
        long double b = Numeric::str_to_ld(args[1]);
        std::string res = "";

        // Perform the calculation based on the operator type.
        if (operator_type == "+")
            res = std::to_string(a + b);
        else if (operator_type == "-")
            res = std::to_string(b - a);
        else if (operator_type == "*")
            res = std::to_string(a * b);
        else if (operator_type == "/")
            res = std::to_string(b / a);
        else if (operator_type == "%")
            res = std::to_string((long long)b % (long long)a);
        else if (operator_type == "^")
            res = std::to_string((long long)a ^ (long long)b);
        else if (operator_type == "&")
            res = std::to_string((long long)a & (long long)b);
        else if (operator_type == "|")
            res = std::to_string((long long)a | (long long)b);
        else if (operator_type == "<<")
            res = std::to_string((long long)b << (long long)a);
        else if (operator_type == ">>")
            res = std::to_string((long long)b >> (long long)a);
        else if (operator_type == "**")
            res = std::to_string(std::pow(b, a));

        return res;
    }
};

// Represents a generic function for the RPN calculator.
// This class implements the IFunctionInfo interface and allows registration of custom functions
// by providing a function pointer for the calculation logic and specifying the number of parameters.
class GenericFunc : public IFunctionInfo
{
    // Pointer to the function that performs the calculation.
    std::string(*calculate_func)(const std::vector<std::string>& args);

    // Number of parameters the function takes.
    int n_parameters;
public:
    // Constructs a GenericFunc with the specified number of parameters and calculation function.
    // Args:
    // - parameters: The number of parameters the function takes.
    // - p_calculate_func: Pointer to the function that performs the calculation.
    GenericFunc(int parameters, std::string(*p_calculate_func)(const std::vector<std::string>& args))
    {
        n_parameters = parameters;
        calculate_func = p_calculate_func;
    }

    // Returns the number of parameters the function takes.
    virtual int num_parameters()
    {
        return n_parameters;
    }

    // Performs the calculation for the function given its arguments.
    // Args:
    // - args: A vector of strings representing the arguments for the function.
    // Returns:
    // - A string representing the result of the calculation.
    virtual std::string calculate(const std::vector<std::string>& args)
    {
        return calculate_func(args);
    }
};

class AverageFunc : public IFunctionInfo
{
public:
    AverageFunc()
    {
    }
    virtual int num_parameters()
    {
        return -1;
    }

    virtual std::string calculate(const std::vector<std::string>& args)
    {
        long double sum = 0;
        for (std::string arg : args)
        {
            long double val = Numeric::str_to_ld(arg);
            sum += val;
        }
        return std::to_string(sum / args.size());
    }

};

void RPN_API RpnCalculator::addFunction(std::string const& name, function_ptr_t functionUniquePtr)
{
    calc->add_function(name, std::move(functionUniquePtr));
}

void RPN_API RpnCalculator::addOperator(std::string const& name, operator_ptr_t operatorUniquePtr)
{
    calc->add_operator(name, std::move(operatorUniquePtr));
}

void RPN_API RpnCalculator::addStandardFunctions()
{
    calc->add_function("g2r", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(Numeric::str_to_ld(args[0]) * std::acos(-1.0) / 180.0); })));
    calc->add_function("r2g", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(Numeric::str_to_ld(args[0]) * 180.0 / std::acos(-1.0)); })));
    calc->add_function("sin", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::sin(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("asin", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::asin(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("cos", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::cos(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("acos", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::acos(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("tan", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::tan(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("atan", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::atan(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("ln", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::log(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("lg", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::log10(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("expn", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::exp(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("hex", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { std::stringstream ss; ss << std::hex << (long long)Numeric::str_to_ld(args[0]); return ss.str();    return ss.str(); })));
    calc->add_function("oct", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { std::stringstream ss; ss << std::oct << (long long)Numeric::str_to_ld(args[0]); return ss.str();    return ss.str(); })));
    calc->add_function("bin", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { std::stringstream ss; ss << std::bitset<64>((long long)Numeric::str_to_ld(args[0])); return ss.str();    return ss.str(); })));

    calc->add_function("sqrt", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::sqrt(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("abs", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::abs(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("floor", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::floor(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("ceil", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(std::ceil(Numeric::str_to_ld(args[0]))); })));
    calc->add_function("c2f", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string(1.8 * (Numeric::str_to_ld(args[0])) + 32); })));
    calc->add_function("f2c", std::unique_ptr<GenericFunc>(new GenericFunc(1, [](const std::vector<std::string>& args) { return std::to_string((Numeric::str_to_ld(args[0]) - 32)/1.8); })));

    calc->add_function("log", std::unique_ptr<GenericFunc>(new GenericFunc(2, [](const std::vector<std::string>& args) { return std::to_string(std::log(Numeric::str_to_ld(args[0])) / std::log(Numeric::str_to_ld(args[1]))); })));
    calc->add_function("pow", std::unique_ptr<GenericFunc>(new GenericFunc(2, [](const std::vector<std::string>& args) { return std::to_string(std::pow(Numeric::str_to_ld(args[0]), Numeric::str_to_ld(args[1]))); })));
    calc->add_function("avg", std::unique_ptr<AverageFunc>(new AverageFunc()));
}

void RPN_API RpnCalculator::addStandardOperators()
{
    calc->add_operator("+", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("+"))));
    calc->add_operator("-", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("-"))));
    calc->add_operator("*", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("*"))));
    calc->add_operator("**", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("**"))));
    calc->add_operator("/", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("/"))));
    calc->add_operator("%", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("%"))));
    calc->add_operator("|", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("|"))));
    calc->add_operator("&", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("&"))));
    calc->add_operator("^", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("^"))));
    calc->add_operator(">>", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string(">>"))));
    calc->add_operator("<<", std::unique_ptr<ArithmeticOperator>(new ArithmeticOperator(std::string("<<"))));

}

RpnCalculator::RpnCalculator(int _verbose) : verbose(_verbose)
{
    calc = new FunctionShuntingYard();
}

RpnCalculator::~RpnCalculator()
{
    delete calc;
}

std::string RPN_API RpnCalculator::calculate_rpn(const std::string &input_rpn)
{
    std::vector<std::string> rpn = calc->tokenize(input_rpn, true);
    if (verbose)
        calc->printRPN(rpn);
    std::string res = calc->evaluateRPN(rpn);
    return res;
}

std::string RPN_API RpnCalculator::calculate(const std::string &input)
{
    std::vector<std::string> rpn = calc->infixToRPN(input);
    if (verbose)
        calc->printRPN(rpn);
    std::string res = calc->evaluateRPN(rpn);
    return res;
}
