#pragma once

namespace NetflixCached {

  /*
   * Enumeration of the commands supported by the server
   */
  enum class RequestType {
    GET,            // Get request
    SET,            // Set request
    NOT_SUPPORTED,  // Unknown request
  };

  /*
   * Enumeration of all the operation codes in the system
   */
  enum class OpCode {
    OK,     // The operation has completed successfully
    ERROR,  // The operation had errors while executing
            // please look at additional structures for more
            // information.
  };

  /*
   * Enumeration of all status codes supported by the cache server
   */
  enum class Status {
    STORED,       // Values has been successfully stored
    ERROR,        // Client submitted a nonexistent command name
    CLIENT_ERROR, // Error in the client submitted input string
    SERVER_ERROR, // Error on server
    VALUE,        // The requested string has been found, and the stored key,flags
                  // and data block are returned, of the specified length
    DEFAULT,      // NB*: This is for internal book-keeping, but never be returned
                  // to client

  };

} // namespace NetflixCached
