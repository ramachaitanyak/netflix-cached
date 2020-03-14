#include <iostream> // For Cout
#include "parser.h" // For Parser API declartions

namespace NetflixCached {

std::pair<NetflixCached::OpCode, std::pair<std::string, std::string>>
  Parser::parseRequest(const std::string network_buffer_input) {
    std::cout<<network_buffer_input<<std::endl;
    return std::make_pair(OpCode::OK, std::make_pair("hello", "hello"));
  }

} // end NetflixCached
