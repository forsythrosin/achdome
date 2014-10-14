#pragma once

#include "jsonActionResolver.h"
#include <string>

JsonActionResolver::JsonActionResolver() {

}

JsonActionResolver::~JsonActionResolver() {

}

bool JsonActionResolver::resolve(std::string jsonMessage, Action &action) {
	std::cout << "Resolving " << jsonMessage << std::endl;
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

	if (!v.contains("message")) {
		std::cout << "Object does not contain a message" << std::endl;
		return false;
	}

	picojson::value message = v.get("message");
	if (!message.is<std::string>()) {
		std::cout << "Message is not a string" << std::endl;
		return false;
	}

	std::string msg = message.get<std::string>();
	std::transform(msg.begin(), msg.end(), msg.begin(), ::tolower);
	if (msg == "register") {
		picojson::value data = v.get("data");
		if (!data.is<picojson::object>()) {
			std::cout << "Data is not an object" << std::endl;
			return false;
		}
		std::string key = "name";
		if (!data.contains(key)) {
			std::cout << key << " is missing" << std::endl;
			return false;
		}
		picojson::value value = data.get(key);
		if (!value.is<std::string>()) {
			std::cout << key << " is not a string" << std::endl;
			return false;
		}
		action.data.insert(std::pair<std::string,std::string>(key, value.to_str()));
		action.type = REGISTER;
		return true;
	} else if (msg == "start") {
		action.type = START;
		return true;
	} else if (msg == "left_down") {
		action.type = LEFT_DOWN;
		return true;
	} else if (msg == "left_up") {
		action.type = LEFT_UP;
		return true;
	} else if (msg == "right_down") {
		action.type = RIGHT_DOWN;
		return true;
	} else if (msg == "right_up") {
		action.type = RIGHT_UP;
		return true;
	}
	std::cout << "Message not recognized: <" << msg << ">" << std::endl;
	return false;
}