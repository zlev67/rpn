#pragma once

#include "IRpnItem.h"
#include <string>


class RpnCalculator
{
	private:
		RpnStack m_rpnStack;
		std::vector <std::unique_ptr<IRpnItemFactory>> m_rpnItemFactories;
		const char* join(const std::vector<std::string>& vec, const std::string& delimiter);
		void tokensCalculate(std::vector<std::string> tokens);
		int findSubTokens(std::vector<std::string> tokens, std::string openToken, std::string closeToken);
		std::vector<std::string> tokensToRpn(std::vector<std::string> tokens);


	public:
		RpnCalculator();
		~RpnCalculator() = default;
		// Push an item onto the RPN stack
		void pushItem(const RpnItemPtr item) {
			//m_rpnStack.push(std::move(item));
		}
		RpnItemPtr rpnOperatorsFactory(std::string token);

		// Calculate the result of the RPN expression
		void calculate(std::string input);

		void clear() {
			while (!m_rpnStack.empty()) {
				m_rpnStack.pop();
			}
		}
		std::vector<std::string> stringToStack(const std::string &expression);
		void visualToRpn(std::string expression);
};

