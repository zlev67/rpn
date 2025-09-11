#pragma once
#include "IRpnItem.h"
#include <algorithm>
#include <cctype>
#include <string>

class RpnItemNumeric : public IRpnItem
{
public:
	RpnItemNumeric(std::string token)
	{
	} 
	virtual ~RpnItemNumeric() = default;
	// Returns the value of the item as a double

	static std::string toUpper(const std::string& input)
	{
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) { return std::toupper(c); });
		return result;
	}
	virtual void calculate(std::string token, RpnStack stack) const override
	{
		std::string data = token;
		size_t idx = 0;
		data = toUpper(data);
		if ((data.find('H') != std::string::npos) ||
			(data.find('X') != std::string::npos)){
			long long res = std::stoll(data, &idx, 16);
			stack.push(std::to_string(res));
			return;
		}
		if (data.find('O') != std::string::npos) {
			long long res = std::stoll(data, &idx, 8);
			stack.push(std::to_string(res));
			return;
		}
		if (data.find('B') != std::string::npos) {
			long long res = std::stoll(data, &idx, 2);
			stack.push(std::to_string(res));
			return;
		}
		stack.push(data);
	}
};

class RpnItemFactoryNumeric : public IRpnItemFactory
{
public:

	virtual ~RpnItemFactoryNumeric() = default;
	// Create an IRpnItem based on the token
	virtual RpnItemPtr create(const std::string& token) const override
	{
		return std::make_unique<RpnItemNumeric>(token);
	}
	virtual std::string getName() const override
	{
		return "Numeric";
	}
	virtual std::vector<int> getParams() const override
	{
		return { 0 };
	}

};
