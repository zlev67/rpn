#pragma once
#include <string>

class WinInetGet
{
	static std::string get_last_error_string();
public:
	std::string get_request(std::wstring url);

};

