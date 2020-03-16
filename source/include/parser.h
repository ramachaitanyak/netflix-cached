#pragma once
#include "status.h" // For NetflixCached::RequestType
#include <vector> // For std::vector

namespace NetflixCached {

/*
 * This interface is exchanged between the cache and parser module
 *
 */
class ParsedPayload {
public:
  ~ParsedPayload() = default;
  // Request type on success is either 'get' or 'set'
  NetflixCached::RequestType request_type;

  // The key. All data is stored using a the specific
  // key. The key cannot contain control characters or
  // whitespace, and can be up to 250 characters in
  // size.
  // This is only meaningful on a 'SET' request_type
  std::string set_key;

  // Vector of keys on a 'GET' request_type
  std::vector<std::string> get_keys;

  // Flags in memcached are transparent
  // This is only meaningful on a 'SET' request_type
  uint32_t flags;

  // The expiry time, or zero for no expiry.
  // This is only meaningful on a 'SET' request_type
  uint32_t exptime;

  // The length of the supplied value block in bytes,
  // excluding the terminating \r\n characters.
  // This is only meaningful on a 'SET' request_type
  int length;

  // Tells the server not to reply to the command.
  // This is only meaningful on a 'SET' request_type
  bool noreply;

  // Unstructured data, which is identified by the
  // key
  // This is only meaningful on a 'SET' request_type
  std::string unstructured_data;
};

class Parser {
public:
  Parser(){}

  ~Parser() = default;

  std::pair<NetflixCached::OpCode, std::pair<NetflixCached::Status, NetflixCached::ParsedPayload>>
    parseRequest(const std::string);

private:
  /*
   * API to parse the type of request
   * @param 'input', this is the request received on the socket
   * @return 'NetflixCached::OpCode' and 'NetflixCached::RequestType',
   *         return code for the operation along with the type of header.
   *         'NetflixCached::OpCode::OK' is returned on successfully extracting the
   *         request type.
   *         'NetflixCached::OpCode::ERROR' is returend on any error encountered.
   *         When this error is returned, 'NetflixCached::RequestType::NOT_SUPPORTED'
   *         is returned along with it.
   */
  std::pair<NetflixCached::OpCode,NetflixCached::RequestType>
    parseRequestType(const std::string& input);

  /*
   * API to parse 'set' payload of request
   * @param 'input', this is the network input excluding the request type, as this is
   *        invoked post 'parseRequestType'. The former API performs error checking and
   *        does not invoke the parsePayload API which is more expensive on errors.
   * @param 'parsed_payload', a reference to the ParsedPayload instance, which will be
   *        populated during the call
   * @return 'NetflixCached::Status', the status of the parsing errors to identify any error
   *          upon no error 'Status::DEFAULT' is returned
   */
  NetflixCached::Status parseSetPayload(const std::string input, ParsedPayload& payload);

  /*
   * API to parse 'get' payload of request
   * @param 'input', this is the network input excluding the request type, as this is
   *        invoked post 'parseRequestType'. The former API performs error checking and
   *        does not invoke the parsePayload API which is more expensive on errors.
   * @param 'parsed_payload', a reference to the ParsedPayload instance, which will be
   *        populated during the call
   * @return 'NetflixCached::Status', the status of the parsing errors to identify any error
   *          upon no error 'Status::DEFAULT' is returned
   */
  NetflixCached::Status parseGetPayload(const std::string input, ParsedPayload& payload);
};
} // end NetflixCached
