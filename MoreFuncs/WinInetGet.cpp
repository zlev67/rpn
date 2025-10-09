#include "WinInetGet.h"


// get_usd_to_nis_wininet.cpp
#include <windows.h>
#include <wininet.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#pragma comment(lib, "wininet")

std::string WinInetGet::get_last_error_string() 
{
    DWORD err = ::GetLastError();
    if (err == 0) return "No error";
    LPWSTR buf = nullptr;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&buf), 0, nullptr);
    std::wstring w(err == 0 ? L"" : buf ? buf : L"Unknown error");
    if (buf) LocalFree(buf);
    // convert wstring to string (simple narrow)
    std::string s;
    s.reserve(w.size());
    for (wchar_t wc : w) s.push_back(static_cast<char>(wc < 0x80 ? wc : '?'));
    return s;
}

std::string WinInetGet::get_request(std::wstring url)
{
    HINTERNET hInternet = nullptr;
    HINTERNET hFile = nullptr;
    std::string response;
    DWORD timeout = 10000;
    hInternet = InternetOpen(L"Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) {
        throw std::runtime_error("InternetOpenW failed: " + get_last_error_string());
    }

    // Optionally set timeouts (connect and receive)
    InternetSetOption(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOption(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOption(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    // Use InternetOpenUrlW (simpler) — it will perform HTTPS automatically when scheme is https
    hFile = InternetOpenUrl(hInternet, url.c_str(), nullptr, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hFile) {
        std::string err = "InternetOpenUrlW failed: " + get_last_error_string();
        InternetCloseHandle(hInternet);
        throw std::runtime_error(err);
    }

    // Read response in chunks
    const DWORD bufSize = 8192;
    std::vector<char> buffer(bufSize);
    DWORD bytesRead = 0;
    while (true) {
        BOOL ok = InternetReadFile(hFile, buffer.data(), bufSize, &bytesRead);
        if (!ok) {
            std::string err = "InternetReadFile failed: " + get_last_error_string();
            InternetCloseHandle(hFile);
            InternetCloseHandle(hInternet);
            throw std::runtime_error(err);
        }
        if (bytesRead == 0) break; // EOF
        response.append(buffer.data(), buffer.data() + bytesRead);
    }

    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);

    return response;
}


//    // Simple JSON parsing: find "rates" then "ILS": value
//    // This is a lightweight approach — for production prefer a JSON parser (nlohmann::json).
//    const std::string key_rates = "\"rates\"";
//    size_t pos_rates = response.find(key_rates);
//    if (pos_rates == std::string::npos) {
//        throw std::runtime_error("Response does not contain 'rates' field. Full response: " + response);
//    }
//
//    // Find "ILS"
//    const std::string key_ils = "\"ILS\"";
//    size_t pos_ils = response.find(key_ils, pos_rates);
//    if (pos_ils == std::string::npos) {
//        // Some APIs use lowercase keys or different structure — try a broader search
//        pos_ils = response.find("ils", pos_rates);
//        if (pos_ils == std::string::npos) {
//            throw std::runtime_error("'ILS' not found in rates. Full response: " + response);
//        }
//    }
//
//    // Find the colon after "ILS"
//    size_t colon = response.find(':', pos_ils);
//    if (colon == std::string::npos) {
//        throw std::runtime_error("Malformed JSON near 'ILS'. Full response: " + response);
//    }
//    // Extract number substring after colon
//    size_t start = colon + 1;
//    // skip whitespace
//    while (start < response.size() && isspace(static_cast<unsigned char>(response[start]))) ++start;
//    // Accept optional sign and digits, decimal point, exponent
//    size_t end = start;
//    bool seenDigit = false;
//    bool seenDot = false;
//    bool seenExp = false;
//    while (end < response.size()) {
//        char c = response[end];
//        if ((c >= '0' && c <= '9')) { seenDigit = true; ++end; continue; }
//        if (!seenDot && c == '.') { seenDot = true; ++end; continue; }
//        if (!seenExp && (c == 'e' || c == 'E')) {
//            seenExp = true; ++end;
//            // optional sign after e
//            if (end < response.size() && (response[end] == '+' || response[end] == '-')) ++end;
//            continue;
//        }
//        // stop at comma or closing brace/bracket or whitespace
//        if (c == ',' || c == '}' || isspace(static_cast<unsigned char>(c))) break;
//        // if unexpected char, break
//        break;
//    }
//    if (!seenDigit || end <= start) {
//        throw std::runtime_error("Failed to parse numeric rate for ILS. Full response: " + response);
//    }
//    std::string numstr = response.substr(start, end - start);
//
//    // Convert string to double
//    try {
//        size_t idx = 0;
//        double val = std::stod(numstr, &idx);
//        (void)idx;
//        return val;
//    }
//    catch (const std::exception& ex) {
//        throw std::runtime_error(std::string("Failed to convert ILS rate to double: ") + ex.what() + " numstr=" + numstr);
//    }
//}

//// Example usage
//int main() {
//const wchar_t* url = L"https://open.er-api.com/v6/latest/USD";
//    try {
//        double rate = get_usd_to_nis_wininet();
//        std::cout << "USD -> ILS = " << rate << std::endl;
//    }
//    catch (const std::exception& ex) {
//        std::cerr << "Error fetching rate: " << ex.what() << std::endl;
//        return 1;
//    }
//    return 0;
//}

