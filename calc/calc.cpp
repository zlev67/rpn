// calc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "RpnCalculator.h" // Include your RPN logic
#include "RpnDef.h"
#include "../MoreFuncs/CurrencyAndStock.h"

bool enumerate_func_callback(std::string const& name, IFunctionInfo const * info)
{
    std::cout << "  " << name << "(";
    if (info->num_parameters() == -1)
        std::cout << "variable parameters";
    else {
        for (int i = 0; i < info->num_parameters(); ++i) {
            if (i > 0)
                std::cout << ", ";
            std::cout << "param" << (i + 1);
        }
    }
    std::cout << ")" << info->description() << std::endl;
    return false;
}


int main(int argc, char *argv[])
{
    if (argc <= 1)
        printf("Command line: c <infix expression> or c -r <RPN expression\n"
               "c -l for list of supported functions.");
    RpnCalculator *calc = new RpnCalculator();
    calc->addStandardFunctions();
    calc->addStandardOperators();
    calc->addFunction("currency", std::unique_ptr<Currency>(new Currency()));
    calc->addFunction("stock", std::unique_ptr<Stock>(new Stock()));
    int cmdIndex = 1; 
    std::string result;
    if (std::string(argv[1]) != "-l")
        calc->enumerateFunctions(enumerate_func_callback);
    else if (std::string(argv[1])!="-r")
        result = calc->calculate(argv[1]);
    else
        result = calc->calculate_rpn(argv[2]);

    delete calc;
    std::cout << result <<"\n";
}

