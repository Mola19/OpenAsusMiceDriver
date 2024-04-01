#include <cstdint>
#include <iostream>
#include <string>

#include "AsusMiceDriver.hpp"
#include "hidapi.h"

AsusMiceDriver::AsusMiceDriver (std::string name, hid_device* hiddev, uint16_t pid) {
    this->name = name;
    device = hiddev;
	config = asus_mice_config[pid];
}

AsusMiceDriver::~AsusMiceDriver() {
    hid_close(device);
}

AsusMiceDriver::DeviceInfo AsusMiceDriver::get_device_info () {
	uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;

    hid_write(device, req, 65);

    unsigned char res[65];
    hid_read(device, res, 65);

    std::string version;
    std::string dongle_version;

    switch(config.version_type)
    {
        case 0:
            {
                unsigned char* offset = res + (config.is_wireless ? 13 : 4);
                version = std::string(offset, offset + 4);
            }
            break;


        case 1:
            {
                unsigned char* offset = res + (config.is_wireless ? 13 : 4);
                version = std::string(offset, offset + 4);
                version = "0." + version.substr(0, 2) + "." + version.substr(2, 2);
            }
            break;

        case 2:
            {
                char _ver[16];
                int offset = (config.is_wireless ? 13 : 4);
                snprintf(_ver, 16, "%2d.%02d.%02d", res[offset + 1], res[offset + 2], res[offset + 3]);
                version = std::string(_ver);
            }
            break;

        case 3:
        case 4:
            {
                char _ver[9];
                char _dgl_ver[9];
                int wireless_offset = (config.version_type == 4 ? 14 : 13);

                int offset = (config.is_wireless ? wireless_offset : 4);
                snprintf(_ver, 9, "%2X.%02X.%02X", res[offset + 2], res[offset + 1], res[offset]);
                version = std::string(_ver);

                snprintf(_dgl_ver, 9, "%2X.%02X.%02X", res[6], res[5], res[4]);
                dongle_version = std::string(_dgl_ver);
            }
            break;
    }

	if (!config.is_wireless)  dongle_version = "";

	DeviceInfo info;

	info.version 			= version;
	info.dongle_version 	= dongle_version;
	info.profile_size		= res[8];
	info.mode_size			= res[9];
	info.active_profile		= res[10];
	info.active_dpi_index	= res[11];
	info.active_dpi			= res[12]; // TODO

	return info;
}

void AsusMiceDriver::set_profile (uint8_t profile) {
    uint8_t req[65];
    memset(req, 0x00, sizeof(req));

    req[0x00]   = 0x00;
    req[0x01]   = 0x50;
    req[0x02]   = 0x02;
    req[0x03]   = profile;

    hid_write(device, req, 65);
}

void AsusMiceDriver::send_data (uint8_t* bytes, size_t size) {

}

void AsusMiceDriver::read_data (uint8_t* bytes, size_t size) {

}
