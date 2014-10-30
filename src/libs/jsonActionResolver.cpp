#include "jsonActionResolver.h"
#include <string>

JsonActionResolver::JsonActionResolver() {

}

JsonActionResolver::~JsonActionResolver() {

}

bool JsonActionResolver::resolve(std::string jsonMessage, ClientAction &action) {
  const char *json = jsonMessage.c_str();
  picojson::value v;
  std::string err;
  picojson::parse(v, json, json + strlen(json), &err);
  if (!err.empty()) {
    std::cout << "Error while parsing json: " << err << std::endl;
    return false;
  }
  if (!v.is<picojson::object>()) {
    std::cout << "JSON is not an object" << jsonMessage << std::endl;
    return false;
  }

  std::string message;
  if (!getString(v, "message", message)) {
    return false;
  }

  std::transform(message.begin(), message.end(), message.begin(), ::tolower);
  if (message == "start_game") {
    // Requires password
    if (!requireDataString(v, "password", action)) return false;
    action.type = ClientAction::START_GAME;
    return true;
  } else if (message == "end_game") {
    // Requires password
    if (!requireDataString(v, "password", action)) return false;
    action.type = ClientAction::END_GAME;
    return true;
  } else if (message == "authenticate_admin") {
    // Requires password
    if (!requireDataString(v, "password", action)) return false;
    action.type = ClientAction::AUTHENTICATE_ADMIN;
    return true;
  } else if (message == "register") {
    // Requires name
		picojson::value data;
		if (!getObject(v, "data", data)) {
			return false;
		}
		std::string name;
		if (!getString(data, "name", name)) {
			return false;
		}
		action.data.insert(std::pair<std::string, std::string>("name", name));
    action.type = ClientAction::REGISTER;
		return true;
  } else if (message == "unregister") {
    action.type = ClientAction::UNREGISTER;
    return true;
  } else if (message == "start_moving") {
    action.type = ClientAction::START_MOVING;
		return true;
	} else if (message == "left_down") {
    action.type = ClientAction::LEFT_DOWN;
		return true;
	} else if (message == "left_up") {
    action.type = ClientAction::LEFT_UP;
		return true;
	} else if (message == "right_down") {
    action.type = ClientAction::RIGHT_DOWN;
		return true;
	} else if (message == "right_up") {
    action.type = ClientAction::RIGHT_UP;
		return true;
	}
	std::cout << "Message not recognized: " << message << std::endl;
	return false;
}

bool JsonActionResolver::requireDataString(picojson::value v, std::string key, ClientAction &action) {
  picojson::value data;
  if (!getObject(v, "data", data)) {
    return false;
  }
  std::string value;
  if (!getString(data, key, value)) {
    return false;
  }
  action.data.insert(std::pair<std::string, std::string>(key, value));
  return true;
}

bool JsonActionResolver::getString(picojson::value v, std::string key, std::string &value) {
	if (!v.contains(key)) {
		std::cout << "Object does not contain any " << key << std::endl;
		return false;
	}

	picojson::value result = v.get(key);
	if (!result.is<std::string>()) {
		std::cout << key << " is not a string" << std::endl;
		return false;
	}
	value = result.get<std::string>();
	return true;
}

bool JsonActionResolver::getObject(picojson::value v, std::string key, picojson::value &value) {
	if (!v.contains(key)) {
		std::cout << "Object does not contain any " << key << std::endl;
		return false;
	}
	picojson::value result = v.get(key);
	if (!result.is<picojson::object>()) {
		std::cout << key << " is not an object" << std::endl;
		return false;
	}
	value = result;
	return true;
}
