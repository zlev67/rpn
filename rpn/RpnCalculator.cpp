#include <regex>
#include "RpnCalculator.h"
#include <windows.h>

#include <sstream>
#include "RpnItemNumeric.h"
#include "RpnItemPlus.h"

char const* RpnCalculator::join(const std::vector<std::string>& vec, const std::string& delimiter)
{
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i != vec.size() - 1)
            oss << delimiter;
    }
    std::string str = oss.str();
    return str.c_str();
}

std::vector<std::string> RpnCalculator::stringToStack(const std::string& input)
{
    std::string expression;
    for (char c : input)
    {
        if (c == ',' || c == '_')
        {
            continue; // Skip whitespace characters
		}
        if (!std::isalnum(static_cast<unsigned char>(c)))
        {
            expression += ' ';
            expression += c;
            expression += ' ';
        }
        else
        {
            expression += c;
        }
    }
    std::regex re(R"(\S+)");

    std::sregex_iterator begin(expression.begin(), expression.end(), re);
    std::sregex_iterator end;

    std::vector<std::string> tokens;
    for (auto it = begin; it != end; ++it) {
        tokens.push_back(it->str());
    }
    return tokens;
}

int RpnCalculator::findSubTokens(std::vector<std::string> tokens, std::string openToken, std::string closeToken)
{
    std::vector<std::string> subTokens;
    int openCount = 0;
    int token_index = 0;
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        token_index = i;
        const auto& token = tokens[i];
        if (token == openToken)
            openCount++;
        else if (token == closeToken)
            openCount--;
        if (openCount == 0)
            break;
    }
    return token_index;
}

void RpnCalculator::tokensCalculate(std::vector<std::string> tokens)
{ 

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const auto& token = tokens[i];
        RpnItemPtr rpnItem = std::move(rpnOperatorsFactory(token));
        if (rpnItem != nullptr)
        {
			rpnItem->calculate(token, m_rpnStack);
        }

          //      if (token == "(")
          //      {
          //          // Get subvector from current position to end
          //          std::vector<std::string> restTokens(tokens.begin() + i, tokens.end());
          //          if (token == "(")
          //              std::vector<std::string> sub = findSubTokens(restTokens, "(", ")")
          //              if (token == "{")
          //                  std::vector<std::string> sub = findSubTokens(restTokens, "{", "}")
          //              if (token == "[")
          //                  std::vector<std::string> sub = findSubTokens(restTokens, "[", "]")

          //       RpnItemPtr rpnItem = rpnOperatorsFactory(token);
		        //m_rpnStack.push(std::move(rpnItem));
    }
    OutputDebugStringA("Tokens extracted from expression:");
    OutputDebugStringA(join(tokens, ","));
    OutputDebugStringA("ended");
}

RpnItemPtr RpnCalculator::rpnOperatorsFactory(std::string token)
{
    // This method should create and return an appropriate IRpnItem based on the current state of the stack
    // For now, this is a placeholder.
    for (const auto& factory : m_rpnItemFactories) {
        RpnItemPtr item = factory->create(token);
        if (item != nullptr)
            return item;
        }
	return nullptr;
}


RpnCalculator::RpnCalculator()
{
    m_rpnItemFactories.push_back(std::make_unique<RpnItemFactoryNumeric>());
    m_rpnItemFactories.push_back(std::make_unique<RpnItemFactoryPlus>());

}

void RpnCalculator::calculate(std::string input)
{
    std::vector<std::string> stack = RpnCalculator::stringToStack(input);
    tokensCalculate(stack);
    OutputDebugStringA("RPN stack is empty after calculation.");
}

std::vector<std::string>  RpnCalculator::tokensToRpn(std::vector<std::string> tokens)
{
    std::vector<std::string> newTokens;
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const auto& token = tokens[i];
        int end_index = i;
        if (token == "(" || token == "{" || token == "[")
        {
            std::vector<std::string> restTokens(tokens.begin() + i, tokens.end());
            // Get subvector from current position to end
            if (token == "(")
                end_index = findSubTokens(restTokens, "(", ")");
            if (token == "{")
                end_index = findSubTokens(restTokens, "{", "}");
            if (token == "[")
                end_index = findSubTokens(restTokens, "[", "]");
            std::vector<std::string> subTokens(tokens.begin() + i, tokens.begin() + end_index);
            tokensToRpn(subTokens);
        }
        else
        {

            newTokens.push_back(token);
        }
    }
    return newTokens;
}
void RpnCalculator::visualToRpn(std::string expression)
{
    std::vector<std::string> tokens = stringToStack(expression);
    tokensToRpn(tokens);

    //OutputDebugStringA("Tokens extracted from expression:");
    //OutputDebugStringA(join(tokens, ","));
    //OutputDebugStringA("ended");

}
