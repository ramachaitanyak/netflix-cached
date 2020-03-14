#pragma once
#include <iostream>

namespace NetflixCached {
/*
 * Cache infrastructure to maintain Extent-Store and Evicted-Store
 * This interface defines the 'processRequest' API which will parse
 * the request and 'get'/'put' into the cache while returning the
 * corresponding status.
 */
class Cache {
public:
  virtual void processRequest(const std::pair<int, std::string>);
  ~Cache() = default;

};
}
