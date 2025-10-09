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

#include "ExprToRpn.h"
#include "Numeric.h"



std::vector<std::string> FunctionShuntingYard::tokenize(const std::string& expression, bool rpn_input) {
    std::vector<std::string> tokens;
    std::string cleanExpr = expression;
    if (!rpn_input)
        cleanExpr.erase(std::remove_if(cleanExpr.begin(), cleanExpr.end(), ::isspace), cleanExpr.end());
    const std::regex tokenRegex(
        R"((0[xX][0-9A-Fa-f]+)"        // hex prefix
        R"(|0[oO][0-7]+)"               // octal prefix
        R"(|0[bB][01]+)"                // binary prefix
        R"(|[0-9A-Fa-f]+[hH])"          // hex suffix
        R"(|[0-7]+[oO])"                // octal suffix
        R"(|[01]+[bB])"                 // binary suffix
        R"(|"[^"]*")"                   // quoted strings
        R"([+-]?|\d+\.\d+|\d+\.\d*|\.\d+|\d+)" // decimal/float
        R"(|[+-]?\d+(?:\.\d*)?[eE][+-]?\d+)"         // scientific notation with integer or decimal base
        R"(|[+-]?\.\d+[eE][+-]?\d+)"                 // scientific notation starting with decimal point
        R"(|[a-zA-Z_][a-zA-Z0-9_]*)"   // identifiers
        R"(|==|!=|<=|>=|&&|\|\||<<|>>|\+\+|--|\+|-|\*|/|\^|\(|\)|,|%|<|>|=|&|\||!|\{|\}|\[|\]))"
    );
 
    // First pass: tokenize
    std::sregex_iterator iter(cleanExpr.begin(), cleanExpr.end(), tokenRegex);
    std::sregex_iterator end;
    while (iter != end) {
        std::string token = iter->str();
        if (!token.empty()) tokens.push_back(token);
        ++iter;
    }

    //// Second pass: merge unary minus into numbers
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "-" && i + 1 < tokens.size()) {
            const std::string& next = tokens[i + 1];
            // Check if next token is a number
            bool isNumber = std::regex_match(next,
                std::regex(R"(0[xX][0-9A-Fa-f]+|0[oO][0-7]+|0[bB][01]+|[0-9A-Fa-f]+[hH]|[0-7]+[oO]|[01]+[bB]|\d+\.\d+|\d+\.\d*|\.\d+|\d+)"));

            if (isNumber) {
                // Unary minus if at start or after operator or left parenthesis/bracket/brace
                if (i == 0 || std::regex_match(tokens[i - 1], std::regex(R"(==|!=|<=|>=|&&|\|\||<<|>>|\+\+|--|\+|-|\*|/|\^|,|%|<|>|=|&|\||!|\(|\{|\[)")))
                {
                    tokens[i + 1] = "-" + tokens[i + 1]; // merge
                    tokens.erase(tokens.begin() + i);   // remove unary minus token
                    --i; // stay at current index
                }
            }
        }
    }

    return tokens;
}
std::vector<std::string> FunctionShuntingYard::infixToRPN(const std::string& infix) {
    std::vector<std::string> tokens = tokenize(infix, false);
    std::vector<std::string> output;
    std::stack<std::string> operatorStack;
    std::stack<int> arityStack;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& token = tokens[i];

        if (!isOperator(token) && ((Numeric::isNumber(token))||Numeric::isString(token)))
        {
            output.push_back(token);
        }
        else if (isFunction(token)) 
        {
            operatorStack.push(token);
            arityStack.push(0);
        }
        else if (token == ",") 
        {
            // parameter separator -> pop until left bracket
            while (!operatorStack.empty() && !isLeftBracket(operatorStack.top())) 
            {
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
            }
            else {
                throw std::runtime_error("Mismatched parentheses/brackets/braces");
            }

            // if a function name is on top now -> it's a function call
            if (!operatorStack.empty() && isFunction(operatorStack.top())) {
                std::string func = operatorStack.top();
                int params_count = getFunctionArity(func);
                operatorStack.pop();

                int paramCount = 1;
                if (!arityStack.empty()) {
                    paramCount = arityStack.top() + 1;
                    arityStack.pop();
                }
                if (params_count == -1)
                    output.push_back(std::to_string(paramCount));
                output.push_back(func);
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

std::string FunctionShuntingYard::evaluateRPN(const std::vector<std::string>& rpn) const 
{
    std::stack<std::string> stack;
    for (const std::string& token : rpn) {
        if (Numeric::isNumber(token) || Numeric::isString(token)) {
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
            IOperatorInfo *operator_info = it->second.get();
            std::vector<std::string> operands;
            for (int i = 0; i < operator_info->num_parameters(); i++)
            {
                operands.push_back(stack.top()); stack.pop();
            }
            std::string result = operator_info->calculate(operands);
            stack.push(result);
        }
        else
            if (isFunction(token))
            {
                auto it = functions.find(to_lower(token));
                if (it == functions.end())
                    throw std::runtime_error("Unknown function: " + token);
                int expectedArity = getFunctionArity(token);
                IFunctionInfo* info = it->second.get();

                if (stack.size() < expectedArity && expectedArity!= -1)
                    throw std::runtime_error("Not enough parameters for function " + token);
                if (expectedArity == -1)
                {
                    expectedArity = std::stoi(stack.top());
                    stack.pop();
                }

                std::vector<std::string> params(expectedArity);
                for (int i = expectedArity - 1; i >= 0; --i)
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

void FunctionShuntingYard::printRPN(const std::vector<std::string>& rpn) const {
    for (size_t i = 0; i < rpn.size(); ++i) {
        std::cout << rpn[i] << (i + 1 < rpn.size() ? " " : "");
    }
    std::cout << std::endl;
}

void FunctionShuntingYard::listFunctions() const {
    std::cout << "Available functions:" << std::endl;
    for (const auto& func : functions) {
        std::cout << "  " << func.first << "(";
        if (func.second.get()->num_parameters() == -1)
            std::cout << "variable parameters";
        else {
            for (int i = 0; i < func.second.get()->num_parameters(); ++i) {
                if (i > 0)
                    std::cout << ", ";
                std::cout << "param" << (i + 1);
            }
        }
        std::cout << ")" << std::endl;
    }
}



