#pragma once

#include <vector>

#include "AsusMouseDriver.hpp"

class OpenAsusMouseDriver {
	public:
		OpenAsusMouseDriver();
		~OpenAsusMouseDriver();
		std::vector<AsusMouseDriver*> getDevices () {
			return device_list;
		}
	private:
		std::vector<AsusMouseDriver*> device_list = {};
};