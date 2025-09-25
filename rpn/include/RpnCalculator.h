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
#include "ExprToRpn.h"

#ifdef RPN_EXPORTS
#define RPN_API __declspec(dllexport)
#else
#define RPN_API __declspec(dllimport)
#endif

class RPN_API RpnCalculator
{
	private:
		FunctionShuntingYard *calc;
		int verbose = 0;

	public:
		RpnCalculator(int _verbose = 0);
		virtual ~RpnCalculator() ;
		// Push an item onto the RPN stack
		void addFunction(std::string const& name, function_ptr_t functionUniquePtr);
		void addOperator(std::string const& name, operator_ptr_t operatorUniquePtr);
		void addStandardFunctions();
		void addStandardOperators();
		std::string calculate_rpn(const std::string & input_rpn);
		std::string calculate(const std::string & input);
};

