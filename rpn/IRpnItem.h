#pragma once
#include <stack>
#include <memory>
#include <vector>
#include <string>

class IRpnItem;

typedef std::unique_ptr<IRpnItem> RpnItemPtr;
typedef std::stack<std::string> RpnStack;
typedef std::unique_ptr <std::vector<std::string>> TokensListPtr;

class IRpnItem
{
	public:
		virtual ~IRpnItem() = default;
		// Returns the value of the item as a double
		virtual void calculate(std::string token, RpnStack stack) const = 0;
};

class IRpnItemFactory
{
	public:
		virtual ~IRpnItemFactory() = default;
		// Create an IRpnItem based on the token
		virtual RpnItemPtr create(const std::string &token) const = 0;
		virtual std::string getName() const = 0;
		virtual std::vector<int> getParams() const = 0;
};
