#include <iostream>
#include <string>
#include <sys/socket.h>
#include "cache.h"
#include <unistd.h> // For read

namespace NetflixCached {

void
Cache::getKeyValues(int io_handle, NetflixCached::ParsedPayloadSharedPtr& payload) {

  // STL standard containers like 'unordered_map' which is used as the
  // extent_store guarantees multiple threads reading at the same time
  // or a single thread writing at the same time.
  // (Ref: http://eel.is/c++draft/#containers)
  // With this there is no need to lock the extent_store on retrieving
  // the data.

  // The response that will be sent out to the client
  std::string response;

  // Compute hash for all the keys and retrieve the corresponding values
  // from the extent store
  for (std::size_t i=0; i<payload->get_keys.size(); i++) {
    std::string key = payload->get_keys[i];
    // Compute hash on the key;
    xxh::hash_t<32> hash_key = xxh::xxhash<32>(key);
    // Look up extent_store for the key
    auto it = Cache::extent_store.find(hash_key);
    if (it != extent_store.end()) {
      // Found a value of type vector<Cache::HashNode*>
      // If the size of the vector is not 1; there were
      // collisions, therefore we must perform additional
      // lookup on the vector.
      if (it->second.size() > 1) {
        for (std::size_t i=0; i<it->second.size(); i++) {
          auto* hash_node = it->second[i];
          // Safety check before dereferencing a pointer
          if (hash_node != nullptr) {
            if (key.compare(hash_node->key) == 0) {
              // Found actual key, build appropriate response
              std::string collision_key_resp =
                "VALUE "+ hash_node->key + " " + std::to_string(hash_node->flags) +
                " " + std::to_string(hash_node->exptime) + " " +
                std::to_string(hash_node->length) + " \r\n";
              std::string collision_key_val = hash_node->unstructured_data;
              response = response + collision_key_resp + collision_key_val;
            }
          }
        }
      } else {
        // There is only one value identified by this key
        auto* hash_node = it->second[0];
        // Safety check before dereferencing a pointer
        if (hash_node != nullptr) {
          std::string collision_key_resp =
            "VALUE "+ hash_node->key + " " + std::to_string(hash_node->flags) +
            " " + std::to_string(hash_node->exptime) + " " +
            std::to_string(hash_node->length) + " \r\n";
          std::string collision_key_val = hash_node->unstructured_data;
          response = response + collision_key_resp + collision_key_val;
        }
      }
    }
  }
  // Append "END\r\n" to the response and send to client
  response = response + "END\r\n";
  send(io_handle, response.c_str(), response.size(), 0);
}

NetflixCached::Status
Cache::setKeyValue(NetflixCached::ParsedPayloadSharedPtr& payload) {
  // Allocate memory for a new HashNode before adding to the
  // extent-store
  Cache::HashNode *node = new Cache::HashNode();
  if (node == nullptr) {
    // This means that the server is out of memory
    return Status::SERVER_ERROR;
  }
  node->key = payload->set_key;
  node->flags = payload->flags;
  node->exptime = payload->exptime;
  node->length = payload->length;
  node->unstructured_data = payload->unstructured_data;
  // Generate current time to indicate object's last access
  node->last_accessed_time = std::time(nullptr);
  // Generate hash for the key using xxhash algorithm
  xxh::hash_t<32> key = xxh::xxhash<32>(payload->set_key);
  // Lock the extent-store for look-up and insert
  {
    std::lock_guard<std::mutex> guard(extent_store_mutex);
    auto it = extent_store.find(key);
    if (it == extent_store.end()) {
      // This means that there is no such key found in the
      // extent store. Create a key value pair and insert into
      // extent store.
      std::vector<Cache::HashNode *> node_values;
      node_values.push_back(node);
      extent_store.insert(make_pair(key, node_values));
    } else {
      // This is a potential collision, as we are trying to set a key
      // that has been already added into the extent-store.
      // We handle collisions by separate chaining. So get the vector
      // of node values and add this new node to it.
      it->second.push_back(node);
    }
  }

  return Status::STORED;

}

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
  std::string return_status = "ERROR";

  ParserSharedPtr parse_object = std::make_shared<Parser>();
  std::pair<NetflixCached::OpCode,
    std::pair<NetflixCached::Status, NetflixCached::ParsedPayloadSharedPtr>>
    result = parse_object->parseRequest(request.second);
  if (result.first == OpCode::ERROR) {
    // Read the type of error generated by the parser
    switch (result.second.first)
    {
      case Status::ERROR:
          break;
      case Status::CLIENT_ERROR:
          return_status.clear();
          return_status = "CLIENT_ERROR";
          break;
      default:
          break;
    }
    std::cout<<"sending error"<<std::endl;
    // Append "\r\n" to the return_status and send to client
    return_status = return_status + "\r\n";
    send(request.first, return_status.c_str(), return_status.size(), 0);
  } else {
    // Read the request type, it can only be 'get' or 'set'
    NetflixCached::ParsedPayloadSharedPtr& payload = result.second.second;

    switch (payload->request_type)
    {
      case NetflixCached::RequestType::GET:
        // Since 'get' API is little more elaborate, we choose to
        // reply to the client through the Cache::getKeyValue API
        // by passing the socket file descriptor associated with the
        // client into it
        std::cout<<"Get message recev"<<std::endl;
        getKeyValues(request.first, payload);
        break;
      case NetflixCached::RequestType::SET:
        // Look at the NetlixCached::Status returned by the 'setKeyValue'
        // API and send the reponse to client accordingly.
        NetflixCached::Status status;
        status = setKeyValue(payload);
        if (status == Status::SERVER_ERROR) {
          return_status.clear();
          return_status = "SERVER_ERROR";
        } else {
          return_status.clear();
          return_status = "STORED";
          // Append "\r\n" to the return_status and send to client
          return_status = return_status + "\r\n";
          send(request.first, return_status.c_str(), return_status.size(), 0);
        }
        break;
      case NetflixCached::RequestType::NOT_SUPPORTED:
        // This can never happen here. If so, this is a server error
        return_status.clear();
        return_status = "SERVER_ERROR";
        // Append "\r\n" to the return_status and send to client
        return_status = return_status + "\r\n";
        send(request.first, return_status.c_str(), return_status.size(), 0);
        break;
      default:
        return_status.clear();
        return_status = "SERVER_ERROR";
        // Append "\r\n" to the return_status and send to client
        return_status = return_status + "\r\n";
        send(request.first, return_status.c_str(), return_status.size(), 0);
        break;
    }
  }

  // Continue to read from socket
  std::cout<<"Continuing to read from socket"<<std::endl;
  char buffer[100];
  auto bytes_read = read(request.first,buffer,100);
  std::cout<<"bytes_read"<<bytes_read<<std::endl;
  std::string req = buffer;
  std::string new_request = req.substr(0, bytes_read);
  std::cout<<"new_req "<<new_request<<std::endl;
  if (bytes_read > 0) {
    this->queueWork(request.first, new_request);
  }
}
} // end NetflixCached
