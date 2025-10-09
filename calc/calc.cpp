// calc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "RpnCalculator.h" // Include your RPN logic
#include "../MoreFuncs/CurrencyAndStock.h"


int main(int argc, char *argv[])
{
    if (argc <= 1)
        printf("Command line: c <infix expression> or c -r <RPN expression");
    RpnCalculator *calc = new RpnCalculator();
    calc->addStandardFunctions();
    calc->addStandardOperators();
    calc->addFunction("currency", std::unique_ptr<Currency>(new Currency()));
    calc->addFunction("stock", std::unique_ptr<Stock>(new Stock()));
    int cmdIndex = 1; 
    std::string result;
    if (std::string(argv[1])!="-r")
        result = calc->calculate(argv[1]);
    else
        result = calc->calculate_rpn(argv[2]);

    delete calc;
    std::cout << result <<"\n";
}

