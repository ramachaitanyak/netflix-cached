#pragma once
#include "cache_errors.h"

namespace NetflixCached {
class Parser {
public:
  Parser(){}

  ~Parser() = default;

  std::pair<int, std::pair<std::string, std::string>> parseRequest() { return std::make_pair(1, std::make_pair("hello", "hello")); }

};
} // end NetflixCached
