#include <iostream>
#include <string>
#include "cache.h"
#include "parser.h"

namespace NetflixCached {

void
Cache::processRequest(const std::pair<int, std::string> request) {
  // Pretend we are doing a lot of work
  //std::this_thread::sleep_for(std::chrono::seconds(5));

  // Send a message to the connection
  //std::string response = "Good talking to you\n";
  //send(item.first, response.c_str(), response.size(), 0);

  // Close the connection
  //close(item.first);
  //
  std::string req = request.second;
  std::cout<<"req "<<req;
  std::cout<<"**************"<<std::endl;

  Parser parse_object;
  std::pair<NetflixCached::OpCode,
    std::pair<NetflixCached::Status, NetflixCached::ParsedPayload>>
    result = parse_object.parseRequest(req);
}


} // end NetflixCached
