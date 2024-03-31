#include <iostream>
#include <hidapi.h>

#include "OpenAsusMiceDriver.hpp"
#include "Devices.hpp"
#include "AsusMiceDriver.hpp"

OpenAsusMiceDriver::OpenAsusMiceDriver () {
    int hid_ret = hid_init();

	if (hid_ret == -1) {
		std::cout << "HIDAPI could not init" << std::endl;
		return;
	}

    hid_device_info* dev_list = hid_enumerate(0, 0);

    if (dev_list == NULL) {
		std::cout << "HIDAPI could not fetch devices" << std::endl;
		return;
	}

    for (; dev_list; dev_list = dev_list->next) {
        for (unsigned int device_index = 0; device_index < hid_devices.size(); device_index++) {
            if (hid_devices[device_index].compare(dev_list)) {
                hid_device* dev = hid_open_path(dev_list->path);
                
                if (dev == NULL) {
                    printf("HIDAPI could not open device \"%s\"", hid_devices[device_index].name.c_str());
                    continue;
                }

                AsusMiceDriver* mouse = new AsusMiceDriver(hid_devices[device_index].name, dev, dev_list->product_id);
                device_list.push_back(mouse);
            }
        }
	}

	hid_free_enumeration(dev_list);

	hid_exit();
}

OpenAsusMiceDriver::~OpenAsusMiceDriver () {
    for (AsusMiceDriver* mouse : device_list) {
        mouse->~AsusMiceDriver();
    }
}
