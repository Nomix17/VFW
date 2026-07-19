#ifndef TRIM
#define TRIM

#include <string>
#include <algorithm>
#include <cctype>

inline void ltrim(std::string &str) {
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), [] (int c) {
    return !std::isspace(c);
  }));
}

inline void rtrim(std::string &str) {
  str.erase(std::find_if(str.rbegin(), str.rend(), [](int c) {
    return !std::isspace(c);
  }).base(), str.end());
}

inline void trim(std::string &str) {
  ltrim(str);
  rtrim(str);
}

inline std::string lowerCase(std::string text) {
  std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c){
    return std::tolower(c); 
  });
  return text;
}

#endif
