#pragma once
#include "IRpnItem.h"
#include <string>

class RpnItemPlus : public IRpnItem
{
	public:
		RpnItemPlus(std::string token)
		{
		}
		virtual ~RpnItemPlus() = default;
		// Returns the value of the item as a double
		virtual void calculate(std::string token, RpnStack stack) const override
		{
			std::string right = std::move(stack.top());
			stack.pop();
			std::string left = std::move(stack.top());
			stack.pop();
			// Assuming both left and right are numeric items, we can cast them
			if ((right.find('.') != std::string::npos) ||
				(left.find('.') != std::string::npos))
			{
				long double r = std::stold(right);
				long double l = std::stold(left);
				long double res = l + r;
				stack.push(std::to_string(res));
			}
			else
			{
				long long r = std::stoll(right);
				long long l = std::stoll(left);
				long long res = l + r;
				stack.push(std::to_string(res));
			}
		}
};

class RpnItemFactoryPlus : public IRpnItemFactory
{
public:

	virtual ~RpnItemFactoryPlus() = default;
	// Create an IRpnItem based on the token
	virtual RpnItemPtr create(const std::string& token) const override
	{
		return std::make_unique<RpnItemPlus>(token);
	}
	virtual std::string getName() const override
	{
		return "Plus";
	}
	virtual std::vector<int> getParams() const override
	{
		return { -1, 1 };
	}
};
