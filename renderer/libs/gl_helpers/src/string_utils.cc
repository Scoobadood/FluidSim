#include "string_utils.h"

#include <vector>
#include <sstream>
#include <algorithm>

std::vector<std::string> tokenise(const std::string &line, char dlm) {
  using namespace std;

  // Vector of string to save tokens
  vector<string> tokens;
  stringstream check1(line);
  string intermediate;
  while (getline(check1, intermediate, dlm)) {
    if (!intermediate.empty())
      tokens.push_back(intermediate);
  }
  return tokens;
}

// trim from start (in place)
void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

// trim from end (in place)
void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

// trim from both ends (in place)
void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}

// inplace convert to lowercase
std::string &to_lower(std::string &str) {
  std::transform(str.begin(), str.end(),
                 str.begin(),
                 [](char ch) {
                   return static_cast<char>((ch >= 'A' && ch <= 'Z') ? ch + ('a' - 'A') : ch);
                 });
  return str;
}
