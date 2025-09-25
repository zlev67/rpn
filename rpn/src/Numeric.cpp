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
#include "Numeric.h"


long double Numeric :: str_to_ld(std::string _token)
/**
 * @brief Converts a string representation of a number into a long double.
 *
 * This function takes a string input and determines its numeric value. It supports
 * binary, octal, hexadecimal, and decimal formats. The function automatically detects
 * the format based on prefixes (e.g., "0b" for binary, "0o" for octal, "0x" for hexadecimal)
 * and suffixes (e.g., "b", "o", "x"). If the input is in decimal format, it is directly
 * converted to a long double.
 *
 * @param _token A string representing the number to be converted.
 * @return The numeric value as a long double. Returns 0 if the input is invalid or empty.
 *
 * @note Supported formats:
 * - Binary: Prefix "0b" or suffix "b" (e.g., "0b1010", "1010b").
 * - Octal: Prefix "0o" or suffix "o" (e.g., "0o77", "77o").
 * - Hexadecimal: Prefix "0x" or suffix "x" (e.g., "0x1A", "1Ax").
 * - Decimal: No prefix or suffix (e.g., "123.45" or in form with E like 1.3E-2).
 */
{
    if (_token.empty())
        return false;
    std::string token = _token;
    bool bin = false;
    bool hex = false;
    bool oct = false;
    char const* ch = token.c_str();
    long double res = 0;
    if (ch[0] == '0')
        switch (token[1])
        {
        case 'b':
        case 'B':
            bin = true;
            ch += 2;
            break;
        case 'o':
        case 'O':
            oct = true;
            ch += 2;
            break;
        case 'x':
        case 'X':
            hex = true;
            ch += 2;
            break;
        }
    switch (token[token.length() - 1])
    {
    case 'b':
    case 'B':
        bin = true;
        token.pop_back();
        break;
    case 'o':
    case 'O':
        oct = true;
        token.pop_back();
        break;
    case 'x':
    case 'X':
        hex = true;
        token.pop_back();
        break;
    }
    int radix = hex ? 16 : oct ? 8 : bin ? 2 : 10;
    if (radix != 10)
        res = std::stol(ch, nullptr, radix);
    else
        res = std::stold(ch);
    return res;
}


bool Numeric::isNumber(const std::string& _token)
/**
 * @brief Checks if a string represents a valid number in binary, octal, hexadecimal, or decimal format.
 *
 * This function determines whether the input string is a valid numeric literal.
 * It supports binary (prefix "0b" or suffix "b"), octal (prefix "0o" or suffix "o"),
 * hexadecimal (prefix "0x" or suffix "x"), and decimal formats. The function
 * automatically detects the format based on the string's prefix or suffix and
 * attempts to parse the value accordingly.
 *
 * @param _token The string to check for numeric validity.
 * @return true if the string is a valid number in one of the supported formats, false otherwise.
 *
 * @note Supported formats:
 * - Binary: Prefix "0b" or suffix "b" (e.g., "0b1010", "1010b").
 * - Octal: Prefix "0o" or suffix "o" (e.g., "0o77", "77o").
 * - Hexadecimal: Prefix "0x" or suffix "x" (e.g., "0x1A", "1Ax").
 * - Decimal: No prefix or suffix (e.g., "123.45" or in form with E like 1.3E-2).
 */
{
    if (_token.empty())
        return false;
    std::string token = _token;
    if (_token.length() == 1 && (_token[0] == '+' || _token[0] == '-'))
        return false;
    bool bin = false;
    bool hex = false;
    bool oct = false;
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
        token.pop_back();
        break;
    case 'o':
    case 'O':
        oct = true;
        token.pop_back();
        break;
    case 'x':
    case 'X':
        hex = true;
        token.pop_back();
        break;
    }
    int radix = hex ? 16 : oct ? 8 : bin ? 2 : 10;
    char const* c = token.c_str();
    char* endptr;
    if (radix == 10)
    {
        long double res = std::strtold(c, &endptr);
        (void)res;
    }
    else
    {
        long long res = std::strtoll(c, &endptr, radix);
        (void)res;
    }
    if (endptr == c + token.length())
        return true;

    return false;
}
