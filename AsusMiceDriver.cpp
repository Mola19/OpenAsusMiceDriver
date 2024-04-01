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

AsusMiceDriver::BatteryInfo AsusMiceDriver::get_battery_info () {
    if (!config.has_battery) {
        BatteryInfo info;
        info._is_ok = false;
        return info;
    };

    uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;
    req[0x02]   = 0x07;

    hid_write(device, req, 65);

    unsigned char res[65];
    hid_read(device, res, 65);

    BatteryInfo info;
    info._is_ok = false;

	info.battery_charge = res[4];
	info.time_to_sleep 	= res[5];
	info.warning_at		= res[6];
	info.is_charging	= (bool) res[9];
	info.unknown1		= res[7];
	info.unknown2	    = res[8];

	return info;
}

bool AsusMiceDriver::get_wake_state () {
    if (!config.has_battery) return true;

    uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;
    req[0x02]   = 0x00;
    req[0x03]   = 0x02;

    hid_write(device, req, 65);

    unsigned char res[65];
    hid_read(device, res, 65);

	return (bool) res[4];
}
