#include "ExprToRpn.h"
#include <vector>



class ExprToRpnConverter
{
	std::vector<std::string> output;
	std::stack<std::string> stack;
	bool hex = false;
	bool oct = false;
	bool bin = false;

	bool isNumber(const std::string& token)
	{
		if (token.empty()) 
			return false;
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

public:
	std::vector<std::string> convert(const std::vector<std::string>& expression)
	{
		for (auto& token : expression)
		{
			if (isNumber(token))
				output.push_back(token);

		}
	};
};