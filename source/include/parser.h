#pragma once
#include "status.h"

namespace NetflixCached {
class Parser {
public:
  Parser(){}

  ~Parser() = default;

  std::pair<NetflixCached::OpCode,
    std::pair<std::string, std::string>> parseRequest(const std::string);

};
} // end NetflixCached
