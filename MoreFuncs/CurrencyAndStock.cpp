#include <algorithm>
#include "CurrencyAndStock.h"
#include "WinInetGet.h"
#include <nlohmann/json.hpp>
#include <Windows.h>


using json = nlohmann::json;


Currency::Currency()
{
}

int Currency::num_parameters()
{
    return 2;
}

std::string to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return s;
}

std::wstring Utf8ToWide(const std::string& str) 
{
    if (str.empty()) return std::wstring();
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

std::string prepare_literal(const std::string& _s) 
{
    std::string s = _s;
    s.erase(std::remove(s.begin(), s.end(), '"'), s.end());
    s = to_upper(s);
    return s;
}

std::string Currency::calculate(const std::vector<std::string>& args)
{
    std::string in = prepare_literal(args[0]);
    std::string out = prepare_literal(args[1]);
    std::string response = WinInetGet().get_request(Utf8ToWide(std::string(URL) + in));
    if (response.empty())
        return "Error";
    json jr = json::parse(response);
    if (jr["result"] != "success")
        return "Error";
    json rates = jr["rates"];
    //std::string s = rates.dump(2);
    if (rates.contains(out))
        return std::to_string((double)rates[out]);
    return "Unknown currency: " + out;
}

Stock::Stock()
{
}

int Stock::num_parameters()
{
    return 1;
}

std::string Stock::calculate(const std::vector<std::string>& args)
{
    std::string in = prepare_literal(args[0]);
    std::string response = WinInetGet().get_request(Utf8ToWide(std::string(URL) + in));
    if (response.empty())
        return "Error";
    json jr = json::parse(response);
    std::string s = jr.dump(2);
    if (jr.contains("chart") && jr["chart"].contains("result") && jr["chart"]["result"].is_array())
    {
        json jr0 = jr["chart"]["result"][0];
        if (jr0.contains("meta") && jr0["meta"].contains("regularMarketPrice"))
            return std::to_string((double)jr0["meta"]["regularMarketPrice"]);
    }
    return "Unknown Stock: " + in;
}
