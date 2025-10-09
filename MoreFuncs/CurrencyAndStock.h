#pragma once

#include "Defs.h"
#include "RpnDef.h"
#include <wchar.h>

class MOREFUNCS_API Currency :
    public IFunctionInfo
{
    char const *URL = "https://open.er-api.com/v6/latest/";
public:
    Currency();
    virtual int num_parameters();

    virtual std::string calculate(const std::vector<std::string>& args);
};

class MOREFUNCS_API Stock :
    public IFunctionInfo
{
    char const *URL = "https://query2.finance.yahoo.com/v8/finance/chart/";
public:
    Stock();
    virtual int num_parameters();

    virtual std::string calculate(const std::vector<std::string>& args);
};

