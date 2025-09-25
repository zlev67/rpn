#include "CppUnitTest.h"
#include "RpnCalculator.h"
#include "ExprToRpn.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(TestMethod3)
		{

            std::vector<std::string> testExpressions = {
                "25 + avg(1, 2, 3, 4, 5) + 15",
                "-1+2+3",
                "-1E+3+21 - 1"
                "1+1*[3*(2+3*{1+3})]",
                "0x5<<1",
                "sin(3.14159 / 2)",
                "sin(g2r(90))",
                "cos(0) + sin(1.5708)",
                "max(5, 3) * min(2, 4)",
                "sqrt(pow(3, 2) + pow(4, 2))",
                "ln(expn(1))",
                "abs(-5) + floor(3.7)",
                "sum(10, 20, 30)",
                "sin(max(1, 2)) + cos(min(3, 4))",
                "pow(2, 3) + sqrt(16)",
                "atan2(1, 1) * 4",  // Should be approximately Pi
                "3 + sin(0) * 2 + max(1, 5)"
            };

            RpnCalculator calc(1);
            for (const std::string& expr : testExpressions) {
                try {
                    std::cout << "Infix:  " << expr << std::endl;
                    std::string out = calc.calculate(expr);
                    std::cout << out << std::endl;
                }
                catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << std::endl;
                    std::cout << "------------------------" << std::endl;
                }
            }
        }

		//TEST_METHOD(TestMethod2)
		//{
		//	RpnCalculator calculator;
		//	calculator.visualToRpn("1+1+1+(2+3+4+5+6)");
		//	// Add more assertions to verify the behavior of the calculator
		//	Assert::IsTrue(true); // Placeholder assertion
		//}

		//TEST_METHOD(TestMethod1)
		//{
		//	RpnCalculator calculator;
		//	calculator.calculate("3 4 +");
		//	calculator.calculate("3.14 1_000_000 *");
		//	// Add more assertions to verify the behavior of the calculator
		//	Assert::IsTrue(true); // Placeholder assertion
		//}
	};
}
