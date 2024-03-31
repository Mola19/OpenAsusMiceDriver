#pragma once

#include <vector>

#include "AsusMiceDriver.hpp"

class OpenAsusMiceDriver {
	public:
		OpenAsusMiceDriver();
		~OpenAsusMiceDriver();
		std::vector<AsusMiceDriver*> getDevices () {
			return device_list;
		}
	private:
		std::vector<AsusMiceDriver*> device_list = {};
};