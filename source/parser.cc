#include <iostream> // For Cout
#include "parser.h" // For Parser API declartions

namespace NetflixCached {

std::pair<NetflixCached::OpCode, NetflixCached::RequestType>
  Parser::parseRequestType(const std::string& input) {

  // The requests are delimited by a " " (space) character
  // Command names are lower-case and are case-sensitive.
  std::string request;
  std::string token = " ";
  size_t found = input.find_first_of(token);
  if (found != std::string::npos) {
    // space found, extract header
    request = input.substr(0, found);
    if (request.compare("get") == 0) {
      return std::make_pair(OpCode::OK, RequestType::GET);
    }
    if (request.compare("set") == 0) {
      return std::make_pair(OpCode::OK, RequestType::SET);
    }
  }

  // Since only 'get' and 'set' are currently supported,
  // return error on all other cases
  return std::make_pair(OpCode::ERROR, RequestType::NOT_SUPPORTED);
}

NetflixCached::Status
Parser::parseSetPayload(std::string input, ParsedPayloadSharedPtr& payload) {

  std::string token = "\r\n";
  size_t found = input.find_first_of(token);
  if (found == std::string::npos) {
    return NetflixCached::Status::CLIENT_ERROR;
  }
  std::string text_line = input.substr(0, found);
  std::string unstructured_data = input.substr(found+2, input.length() - found-2);
  // Make sure that the unstructured data is terminated by "\r\n"
  if (unstructured_data.substr(unstructured_data.length() - 2) != "\r\n") {
    return NetflixCached::Status::CLIENT_ERROR;
  }
  // Parse the text_line to extract the additional parameters for 'set' command
  std::string space_token = " ";
  size_t found_space = text_line.find_first_of(space_token);
  if (found_space == std::string::npos) {
    return NetflixCached::Status::CLIENT_ERROR;
  } else {
    int iterator_index = 0;
    while(found_space != std::string::npos) {

      if (iterator_index == 0) {
        std::string key = text_line.substr(0, found_space);
        // Validate key features before setting the key
        // 1. Key can not be of size more than 250 bytes/characters
        // 2. Key can not contain any control characters
        if (key.length() > 250) {
          return NetflixCached::Status::CLIENT_ERROR;
        }
        bool is_control = false;
        for (size_t i = 0; i<key.length(); i++) {
          if (iscntrl(key[i])) {
            is_control = true;
          }
        }
        if (is_control){
          return NetflixCached::Status::CLIENT_ERROR;
        }
        payload->set_key = key;
      }

      if (iterator_index == 1) {
        // Acquire the flags from the text_line
        std::string flags = text_line.substr(0, found_space);
        payload->flags = static_cast<uint32_t>(std::stoul(flags));
      }

      if (iterator_index == 2) {
        // Acquire the exptime from the text_time
        std::string exptime = text_line.substr(0, found_space);
        payload->exptime = static_cast<uint32_t>(std::stoul(exptime));
      }

      if (iterator_index == 3) {
        //Acquire the length of unstructured data excluding "\r\n"
        std::string length = text_line.substr(0, found_space);
        payload->length = std::stoi(length);
      }

      text_line = text_line.substr(found_space + 1, text_line.length() - found_space);
      found_space = text_line.find_first_of(space_token);
      iterator_index++;
    }

    if (iterator_index < 3) {
      // There has been an error in the input supplied by the client
      // End processing and return CLIENT_ERROR
      return NetflixCached::Status::CLIENT_ERROR;
    }

    // Check to see if the 'noreply' argument has been passed into the string
    // as this is optional. If this is not provided the remaining text_line is
    // the payload length after parsing out "\r\n"
    text_line = text_line.substr(0, text_line.length());
    if (text_line.compare("noreply") == 0) {
      payload->noreply = true;
    } else {
      payload->noreply = false;
      //Acquire the length of unstructured data excluding "\r\n"
      std::string length = text_line;
      payload->length = std::stoi(length);
    }
  }

  // Unstructured_data length should be two more than the length specified by the
  // payload accounting for "/r/n"
  if (unstructured_data.length() - payload->length == 2) {
    payload->unstructured_data = unstructured_data;
  } else {
    return NetflixCached::Status::CLIENT_ERROR;
  }

  return NetflixCached::Status::DEFAULT;
}

NetflixCached::Status
Parser::parseGetPayload(std::string text_line, ParsedPayloadSharedPtr& payload) {
  std::string token = "\r\n";
  std::string space_token = " ";
  size_t found = text_line.find_first_of(token);
  if (found == std::string::npos) {
    // This is client input error
    return NetflixCached::Status::CLIENT_ERROR;
  } else {
    size_t found_space = text_line.find_first_of(space_token);
    while(found_space != std::string::npos) {
      // There are more get keys to parse
      std::string key = text_line.substr(0, found_space);
      payload->get_keys.push_back(key);
      text_line = text_line.substr(found_space + 1, text_line.length() - found_space);
      found_space = text_line.find_first_of(space_token);
    }
    //Append the last get key
    payload->get_keys.push_back(text_line.substr(0,text_line.length() - 2));
  }

  return NetflixCached::Status::DEFAULT;
}

std::pair<NetflixCached::OpCode, std::pair<NetflixCached::Status, NetflixCached::ParsedPayloadSharedPtr>>
  Parser::parseRequest(const std::string network_buffer_input) {
  //ParsedPayload payload;
  ParsedPayloadSharedPtr payload = std::make_shared<ParsedPayload>();
  NetflixCached::Status status = Status::ERROR;
  NetflixCached::OpCode op_code = OpCode::OK;
  // Identify the type of request. Since we are only supporting
  // 'get' and 'set'. The size of the request can not be more than
  // 3 bytes.
  auto request_type = parseRequestType(network_buffer_input);
  if (request_type.first == OpCode::ERROR) {
    // Return dummy instance of ParsedPayload and nothing on this request
    return std::make_pair(request_type.first, std::make_pair(Status::ERROR, payload));
  }

  std::string request = network_buffer_input.substr(4, network_buffer_input.length()-4);
  payload->request_type = request_type.second;

  if (request_type.second == RequestType::SET) {
    // Parse the remaining portion of the request
    status = parseSetPayload(request, payload);
    if (status != Status::DEFAULT) {
      op_code = OpCode::ERROR;
    }
  }

  if (request_type.second == RequestType::GET) {
    // Parse the remaining portion of the request
    status = parseGetPayload(request, payload);
    if (status != Status::DEFAULT) {
      op_code = OpCode::ERROR;
    }
  }

  return std::make_pair(op_code, std::make_pair(status, payload));
}

} // end NetflixCached
