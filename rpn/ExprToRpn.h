#pragma once
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

class IOperatorInfo
{
public:
    int precedence;
    int num_parameters;
    virtual std::string calculate(const std::vector<std::string>& args) = 0;
};
class IFunctionInfo
{
public:
    int num_parameters;
    virtual std::string calculate(const std::vector<std::string>& args) = 0;
};


class FunctionShuntingYard {
private:
    typedef std::unique_ptr<IOperatorInfo> operator_ptr_t;
    typedef std::unique_ptr<IFunctionInfo> function_ptr_t;
    std::unordered_map<std::string, operator_ptr_t> operators;
    std::unordered_map<std::string, function_ptr_t> functions;

    //= {
    //    {"+", 1}, {"-", 1},
    //    {"*", 2}, {"/", 2}, {"%", 2},
    //    {"^", 3}
    //};

    std::unordered_set<std::string> rightAssociative = { "^" };

    //std::unordered_map<std::string, std::pair<int, std::function<double(const std::vector<double>&)>>> functions = {
    //    {"sin", {1, [](const std::vector<double>& args) { return std::sin(args[0]); }}},
    //    {"cos", {1, [](const std::vector<double>& args) { return std::cos(args[0]); }}},
    //    {"tan", {1, [](const std::vector<double>& args) { return std::tan(args[0]); }}},
    //    {"log", {1, [](const std::vector<double>& args) { return std::log(args[0]); }}},
    //    {"exp", {1, [](const std::vector<double>& args) { return std::exp(args[0]); }}},
    //    {"sqrt", {1, [](const std::vector<double>& args) { return std::sqrt(args[0]); }}},
    //    {"abs", {1, [](const std::vector<double>& args) { return std::abs(args[0]); }}},
    //    {"floor", {1, [](const std::vector<double>& args) { return std::floor(args[0]); }}},
    //    {"ceil", {1, [](const std::vector<double>& args) { return std::ceil(args[0]); }}},

    //    {"min", {2, [](const std::vector<double>& args) { return std::min(args[0], args[1]); }}},
    //    {"max", {2, [](const std::vector<double>& args) { return std::max(args[0], args[1]); }}},
    //    {"pow", {2, [](const std::vector<double>& args) { return std::pow(args[0], args[1]); }}},
    //    {"atan2", {2, [](const std::vector<double>& args) { return std::atan2(args[0], args[1]); }}},

    //    {"avg", {-1, [](const std::vector<double>& args) {
    //        double sum = 0;
    //        for (double arg : args) sum += arg;
    //        return sum / args.size();
    //    }}},
    //    {"sum", {-1, [](const std::vector<double>& args) {
    //        double sum = 0;
    //        for (double arg : args) sum += arg;
    //        return sum;
    //    }}}
    //};

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

public:
    void add_operator(std::string name, operator_ptr_t operator_info)
    {
        operators[name] = std::move(operator_info);
    }

    void add_function(std::string name, function_ptr_t function_info)
    {
        functions[name] = std::move(function_info);
    }

    // Tokenizer now includes [ ] { }
    std::vector<std::string> tokenize(const std::string& expression) {
        std::vector<std::string> tokens;
        std::string cleanExpr = expression;
        cleanExpr.erase(std::remove_if(cleanExpr.begin(), cleanExpr.end(), ::isspace), cleanExpr.end());

        // include square and curly braces in the token set
        std::regex tokenRegex(R"(\d+\.?\d*|[a-zA-Z_][a-zA-Z0-9_]*|[+\-*/^(),%\[\]\{\}])");

        std::sregex_iterator iter(cleanExpr.begin(), cleanExpr.end(), tokenRegex);
        std::sregex_iterator end;
        while (iter != end) {
            std::string token = iter->str();
            if (!token.empty()) tokens.push_back(token);
            ++iter;
        }
        return tokens;
    }

    bool isOperator(const std::string& token) const {
        return operators.find(token) != operators.end();
    }
    bool isFunction(const std::string& token) const {
        return functions.find(token) != functions.end();
    }
    //bool isNumber(const std::string& token) const 
    //{
    //    if (token.empty()) return false;
    //    try {
    //        size_t pos = 0;
    //        std::stod(token, &pos);
    //        return pos == token.length();
    //    } catch (...) {
    //        return false;
    //    }
    //}
    bool isNumber(const std::string& token) const
    {
        if (token.empty())
            return false;
        bool bin = false;
        bool hex = false;
        bool oct = false;
        if (token[0] == '0')
            switch (token[1])
            {
            case 'b':
            case 'B':
                bin = true;
                break;
            case 'o':
            case 'O':
                oct = true;
                break;
            case 'x':
            case 'X':
                hex = true;
                break;
            }
        switch (token[token.length() - 1])
        {
        case 'b':
        case 'B':
            bin = true;
            break;
        case 'o':
        case 'O':
            oct = true;
            break;
        case 'x':
        case 'X':
            hex = true;
            break;
        }
        int radix = hex ? 16 : oct ? 8 : bin ? 2 : 10;
        for (char const* ch = token.c_str(); *ch; ch++)
        {
            char character = *ch;
            // e(E) and "." are sign of floating point, it cannot be used with hex, octal or binary characters)
            if ((character == '.' || character == 'E') && (hex || oct || bin))
                return false;
            if (!std::isxdigit(character) || character > radix)
                return false;
        }
        return true;
    }

    int getPrecedence(const std::string& op) const {
        auto it = operators.find(op);
        return (it != operators.end()) ? it->second.get()->precedence : 0;
    }
    bool isRightAssociative(const std::string& op) const {
        return rightAssociative.find(op) != rightAssociative.end();
    }
    int getFunctionArity(const std::string& func) const {
        auto it = functions.find(func);
        return (it != functions.end()) ? it->second.get()->num_parameters : 0;
    }

    // Shunting yard with support for (), [], {}
    std::vector<std::string> infixToRPN(const std::string& infix) {
        std::vector<std::string> tokens = tokenize(infix);
        std::vector<std::string> output;
        std::stack<std::string> operatorStack;
        std::stack<int> arityStack;

        for (size_t i = 0; i < tokens.size(); ++i) {
            const std::string& token = tokens[i];

            if (isNumber(token)) {
                output.push_back(token);
            }
            else if (isFunction(token)) {
                operatorStack.push(token);
                arityStack.push(0);
            }
            else if (token == ",") {
                // parameter separator -> pop until left bracket
                while (!operatorStack.empty() && !isLeftBracket(operatorStack.top())) {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();
                }
                if (!arityStack.empty()) {
                    arityStack.top()++;
                }
            }
            else if (isOperator(token)) {
                while (!operatorStack.empty() &&
                       !isLeftBracket(operatorStack.top()) &&
                       !isFunction(operatorStack.top()) &&
                       (getPrecedence(operatorStack.top()) > getPrecedence(token) ||
                        (getPrecedence(operatorStack.top()) == getPrecedence(token) &&
                         !isRightAssociative(token)))) {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.push(token);
            }
            else if (isLeftBracket(token)) {
                operatorStack.push(token);
            }
            else if (isRightBracket(token)) {
                std::string opening = matchingOpening(token);
                // pop until matching opening bracket
                while (!operatorStack.empty() && operatorStack.top() != opening) {
                    // if we hit a different left bracket type -> mismatched
                    if (isLeftBracket(operatorStack.top()) && operatorStack.top() != opening) {
                        throw std::runtime_error("Mismatched parentheses/brackets/braces");
                    }
                    output.push_back(operatorStack.top());
                    operatorStack.pop();
                }
                if (!operatorStack.empty() && operatorStack.top() == opening) {
                    operatorStack.pop(); // remove the matching opening
                } else {
                    throw std::runtime_error("Mismatched parentheses/brackets/braces");
                }

                // if a function name is on top now -> it's a function call
                if (!operatorStack.empty() && isFunction(operatorStack.top())) {
                    std::string func = operatorStack.top();
                    operatorStack.pop();

                    int paramCount = 1;
                    if (!arityStack.empty()) {
                        paramCount = arityStack.top() + 1;
                        arityStack.pop();
                    }
                    output.push_back(func + ":" + std::to_string(paramCount));
                }
            }
            else {
                throw std::runtime_error("Unknown token: " + token);
            }
        }

        while (!operatorStack.empty()) {
            if (isLeftBracket(operatorStack.top()) || isRightBracket(operatorStack.top())) {
                throw std::runtime_error("Mismatched parentheses/brackets/braces");
            }
            output.push_back(operatorStack.top());
            operatorStack.pop();
        }
        return output;
    }

    // RPN evaluator (unchanged)
    std::string evaluateRPN(const std::vector<std::string>& rpn) const {
        std::stack<std::string> stack;
        for (const std::string& token : rpn) {
            if (isNumber(token)) {
                stack.push(token);
            }
            else if (isOperator(token)) 
            {

                if (stack.size() < 2) 
                    throw std::runtime_error("Invalid expression");
                std::string t = token;
                auto it = operators.find(token);
                if (it == operators.end() || !it->second) {
                    throw std::runtime_error("Unknown or uninitialized operator: " + token);
                }
                auto operator_info = it->second.get();
                std::vector<std::string> operands;
                for (int i = 0; i < operator_info->num_parameters; i++)
                {
                    operands.push_back(stack.top()); stack.pop();
                }
                std::string result = operator_info->calculate(operands);
                stack.push(result);
            }
            else 
                if (token.find(':') != std::string::npos) 
                {
                    size_t colonPos = token.find(':');
                    std::string funcName = token.substr(0, colonPos);
                    int paramCount = std::stoi(token.substr(colonPos + 1));
                    auto it = functions.find(funcName);
                    if (it == functions.end()) 
                        throw std::runtime_error("Unknown function: " + funcName);
                    if (stack.size() < static_cast<size_t>(paramCount)) 
                        throw std::runtime_error("Not enough parameters for function " + funcName);
                    IFunctionInfo* info = it->second.get();
                    int expectedArity = info->num_parameters;
                    if (expectedArity != -1 && expectedArity != paramCount) 
                    {
                        throw std::runtime_error("Function " + funcName + " expects " + std::to_string(expectedArity) +
                                                 " parameters, got " + std::to_string(paramCount));
                    }

                    std::vector<std::string> params(paramCount);
                    for (int i = paramCount - 1; i >= 0; --i) 
                    {
                        params[i] = stack.top();
                        stack.pop();
                    }
                    std::string result = info->calculate(params);
                    stack.push(result);
                }
            else 
                {
                    throw std::runtime_error("Unknown token in RPN: " + token);
                }
        }

        if (stack.size() != 1) throw std::runtime_error("Invalid expression");
        return stack.top();
    }

    void printRPN(const std::vector<std::string>& rpn) const {
        for (size_t i = 0; i < rpn.size(); ++i) {
            std::cout << rpn[i] << (i + 1 < rpn.size() ? " " : "");
        }
        std::cout << std::endl;
    }

    void listFunctions() const {
        std::cout << "Available functions:" << std::endl;
        for (const auto& func : functions) {
            std::cout << "  " << func.first << "(";
            if (func.second.get()->num_parameters == -1) 
                std::cout << "variable parameters";
            else {
                for (int i = 0; i < func.second.get()->num_parameters; ++i) {
                    if (i > 0) 
                        std::cout << ", ";
                    std::cout << "param" << (i + 1);
                }
            }
            std::cout << ")" << std::endl;
        }
    }
};


int test_rpn() {
    std::cout << "=== Enhanced Shunting Yard Algorithm with Functions ===" << std::endl;

    FunctionShuntingYard converter;
    converter.listFunctions();
    std::cout << std::endl;


    // Interactive mode
    std::string input;
    std::cout << "\n=== Interactive Mode ===" << std::endl;
    std::cout << "Enter expressions with functions (or 'quit' to exit):" << std::endl;
    std::cout << "Examples: sin(3.14/2), max(5,3), avg(1,2,3,4), sqrt(pow(3,2)+pow(4,2))" << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit") {
            break;
        }

        if (input == "help" || input == "functions") {
            converter.listFunctions();
            continue;
        }

        if (input.empty()) {
            continue;
        }

        try {
            std::vector<std::string> rpn = converter.infixToRPN(input);
            std::cout << "RPN: ";
            converter.printRPN(rpn);

            std::string result = converter.evaluateRPN(rpn);
            std::cout << "Result: " << result << std::endl;

        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}

