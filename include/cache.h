#pragma once
#include <iostream>
/*
 * Cache infrastructure to maintain Extent-Store and Evicted-Store
 * This interface defines the 
 *
 */
class Cache {
public:
  virtual void processRequest(const std::pair<int, std::string>) = 0;
  ~Cache() = default;

};
