#ifndef UTAH_ICG_STRING_UTILS_H
#define UTAH_ICG_STRING_UTILS_H


#include <vector>
#include <string>

std::vector<std::string> tokenise(const std::string &line, char dlm);

// trim from start (in place)
void ltrim(std::string &s);

// trim from end (in place)
void rtrim(std::string &s);

// trim from both ends (in place)
void trim(std::string &s);

// Inplace to lower
std::string &to_lower(std::string &str);

#endif //UTAH_ICG_STRING_UTILS_H
