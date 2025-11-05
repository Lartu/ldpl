/* This file contains auxiliary functions to check data type compliance */
#include <string>
#include <cctype>
#include "../ldpl.h"

bool is_number(const string &number)
{
    if (number.empty())
    {
        return false;
    }
    std::size_t i = 0;
    // Optional sign
    if (number[i] == '+' || number[i] == '-')
    {
        ++i;
        if (i == number.size())
        {
            // String cannot be just "+" or "-"
            return false;
        }
    }
    bool has_digits_before_dot = false;
    bool has_digits_after_dot = false;
    // Digits before decimal point
    while (i < number.size() && std::isdigit(static_cast<unsigned char>(number[i])))
    {
        has_digits_before_dot = true;
        ++i;
    }

    // Optional decimal point and digits after
    if (i < number.size() && number[i] == '.')
    {
        ++i; // skip '.'

        while (i < number.size() && std::isdigit(static_cast<unsigned char>(number[i])))
        {
            has_digits_after_dot = true;
            ++i;
        }
    }
    // If there are leftover characters, it is invalid
    if (i != number.size())
    {
        return false;
    }
    // Cases:
    // 1) Integer: must have digits before dot, and no dot or no digits after dot required.
    // 2) Decimal: must have digits before and after the dot to match x.y
    if (has_digits_before_dot && !has_digits_after_dot)
    {
        // "x" form
        return true;
    }
    if (has_digits_before_dot && has_digits_after_dot)
    {
        // "x.y" form
        return true;
    }
    return false;
}

bool is_natural(const string number)
{
    // -- Checks if a string is a valid natural number --
    if (!is_number(number))
        return false;
    if (stod(number) <= 0)
        return false;
    for (char l : number)
        if (l == '.')
            return false;
    return true;
}

bool is_string(const string &token)
{
    // -- Checks if a string is a valid LDPL string --
    if (token.size() < 2 || token[0] != '"' || token[token.size() - 1] != '"')
        return false;
    // Check for unescaped quotes
    for (unsigned int i = 1; i < token.size() - 1; ++i)
    {
        if (token[i] == '\"' && token[i - 1] != '\\')
            return false;
    }
    return true;
}