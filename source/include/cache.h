#pragma once
#include <iostream>
#include "parser.h"

namespace NetflixCached {
/*
 * Cache infrastructure to maintain Extent-Store and Evicted-Store
 * This interface defines the 'processRequest' API which will parse
 * the request and 'get'/'put' into the cache while returning the
 * corresponding status.
 */
class Cache {
public:
  Cache():i(0) {}
  ~Cache() = default;
  virtual void processRequest(const std::pair<int, std::string>);

private:
  std::pair<NetflixCached::Status, std::pair<std::string, std::string>>
    getKeyValue(const std::string& key) {
      std::cout<<key<<std::endl;
      return std::make_pair(Status::ERROR, std::make_pair("he","sje"));
    }
  NetflixCached::Status setKeyValue(NetflixCached::ParsedPayload& payload) {
    std::cout<<payload.set_key<<std::endl;
    return Status::ERROR;
  }

  int i;


};
}
