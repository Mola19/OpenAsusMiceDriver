#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <vector>
#include <algorithm>

#include <hidapi.h>

#include "AsusMouseDriver.hpp"
#include "ReadCallback.hpp"

AsusMouseDriver::AsusMouseDriver (std::string name, hid_device* hiddev, uint16_t pid) {
    this->name = name;
    device = hiddev;
	config = asus_mouse_config[pid];

    cb = new ReadCallback(hiddev);
}

AsusMouseDriver::~AsusMouseDriver() {
    delete cb;
    hid_close(device);
}

AsusMouseDriver::DeviceInfo AsusMouseDriver::get_device_info () {
	uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;
    req[0x02]   = 0x00;
    req[0x02]   = 0x00;

    hid_write(device, req, 65);

    std::vector<uint8_t> res = await_response(req+1, 3);

    std::string version;
    std::string dongle_version;

    uint8_t* res_arr = &res[0];
    switch(config.version_type)
    {
        case 0:
            {
                uint8_t* offset = res_arr + 4;
                version = std::string(offset, offset + 4);
            }
            break;

        case 1:
            {
                char _ver[16];
                snprintf(_ver, 16, "%2d.%02d.%02d", res[5], res[6], res[7]);
                version = std::string(_ver);
                // no dongle only for non-wireless Strix Impact
            }
            break;

        case 2:
            {
                uint8_t* offset = res_arr + 4;
                version = std::string(offset, offset + 4);
                version = "0." + version.substr(0, 2) + "." + version.substr(2, 2);
                // no dongle only for non-wireless TUF M5
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

void AsusMouseDriver::set_profile (uint8_t profile) {
    uint8_t req[65];
    memset(req, 0x00, sizeof(req));

    req[0x00]   = 0x00;
    req[0x01]   = 0x50;
    req[0x02]   = 0x02;
    req[0x03]   = profile;

    hid_write(device, req, 65);

    await_response(req+1, 3);
}

void AsusMouseDriver::save_current_profile () {
    uint8_t req[65];
    memset(req, 0x00, sizeof(req));

    req[0x00]   = 0x00;
    req[0x01]   = 0x50;
    req[0x02]   = 0x03;

    hid_write(device, req, 65);

    await_response(req+1, 2);
}

void AsusMouseDriver::reset_all_profiles () {
    uint8_t req[65];
    memset(req, 0x00, sizeof(req));

    req[0x00]   = 0x00;
    req[0x01]   = 0x50;
    req[0x02]   = 0x04;

    hid_write(device, req, 65);

    await_response(req+1, 2);
}

void AsusMouseDriver::reset_current_profile () {
    uint8_t req[65];
    memset(req, 0x00, sizeof(req));

    req[0x00]   = 0x00;
    req[0x01]   = 0x50;
    req[0x02]   = 0x05;

    hid_write(device, req, 65);

    await_response(req+1, 2);
}

AsusMouseDriver::BatteryInfo AsusMouseDriver::get_battery_info () {
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

    std::vector<uint8_t> res = await_response(req+1, 2);

    BatteryInfo info;
    info._is_ok = false;

	info.battery_charge = res[4];
	info.time_to_sleep 	= (BatteryTimeToSleepValues) res[5];
	info.warning_at		= res[6];
	info.is_charging	= (bool) res[9];
	info.unknown1		= res[7];
	info.unknown2	    = res[8];

	return info;
}

bool AsusMouseDriver::get_wake_state () {
    if (!config.is_wireless) return true;

    uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;
    req[0x02]   = 0x00;
    req[0x03]   = 0x02;

    hid_write(device, req, 65);

    std::vector<uint8_t> res = await_response(req+1, 3);

	return (bool) res[4];
}

void AsusMouseDriver::set_battery_settings(uint8_t time_to_sleep, uint8_t warning_at) {
    if (!config.has_battery) return;
    
    uint8_t req[65];
    memset(req, 0x00, sizeof(req));

    req[0x00]   = 0x00;
    req[0x01]   = 0x51;
    req[0x02]   = 0x37;
    req[0x03]   = 0x00;
    req[0x04]   = 0x00;
    req[0x05]   = time_to_sleep;
    req[0x06]   = 0x00;
    req[0x07]   = warning_at;

    hid_write(device, req, 65);

    await_response(req+1, 2);
}

AsusMouseDriver::LightingZoneInfo AsusMouseDriver::get_lighting_zone_info (int zone) {
    if (!config.has_lighting) {
        LightingZoneInfo info;
        info._is_ok = false;
        return info;
    };

    if (!config.is_small_packet) return get_lighting_info()[zone];

    uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;
    req[0x02]   = 0x03;
    req[0x03]   = zone;

    hid_write(device, req, 65);

    std::vector<uint8_t> res = await_response(req+1, 2);

    LightingZoneInfo zone_info;
    zone_info._is_ok = true;

    zone_info.mode = config.lighting_modes.at(res[4]);
    zone_info.mode_raw = res[4];
    zone_info.brightness = res[5];
    zone_info.red = res[6];
    zone_info.green = res[7];
    zone_info.blue = res[8];
    zone_info.direction = res[10];
    zone_info.random = res[11];
    zone_info.speed = res[12];

	return zone_info;
}

AsusMouseDriver::LightingZoneInfo AsusMouseDriver::get_dock_lighting () {
    if (!config.has_lighting || !config.has_dock) {
        LightingZoneInfo info;
        info._is_ok = false;
        return info;
    };

    uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;
    req[0x02]   = 0x03;
    req[0x03]   = 0x03;

    hid_write(device, req, 65);

    std::vector<uint8_t> res = await_response(req+1, 2);

    LightingZoneInfo zone_info;
    zone_info._is_ok = true;

    zone_info.mode = config.lighting_modes.at(res[4]);
    zone_info.mode_raw = res[4];
    zone_info.brightness = res[5];
    zone_info.red = res[6];
    zone_info.green = res[7];
    zone_info.blue = res[8];
    zone_info.direction = res[10];
    zone_info.random = res[11];
    zone_info.speed = res[12];

	return zone_info;
}

std::vector<AsusMouseDriver::LightingZoneInfo> AsusMouseDriver::get_lighting_info () {
    if (!config.has_lighting) {
        std::vector<LightingZoneInfo> info_vec = {};
        LightingZoneInfo info;
        info._is_ok = false;
        info_vec.push_back(info);
        return info_vec;
    };

    if (config.is_small_packet) {
        std::vector<LightingZoneInfo> info_vec = {};

        for (int i = 0; i < config.lighting_zones.size(); i++) {
            info_vec.push_back(get_lighting_zone_info(i));
        }

        return info_vec;
    }

    uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;
    req[0x02]   = 0x03;
    req[0x03]   = 0x00;

    hid_write(device, req, 65);

    std::vector<uint8_t> res = await_response(req+1, 3);

    std::vector<LightingZoneInfo> info_vec = {};

    for (int i = 0; i < config.lighting_zones.size(); i++) {
        LightingZoneInfo zone_info;
        zone_info._is_ok = true;

        zone_info.mode = config.lighting_modes.at(res[i*5 + 4]);
        zone_info.mode_raw = res[i*5 + 4];
        zone_info.brightness = res[i*5 + 5];
        zone_info.red = res[i*5 + 6];
        zone_info.green = res[i*5 + 7];
        zone_info.blue = res[i*5 + 8];
        zone_info.direction = res[20];
        zone_info.random = res[21];
        zone_info.speed = res[22];

        info_vec.push_back(zone_info);
    }

    return info_vec;
}

void AsusMouseDriver::set_lighting (uint8_t zone, LightingZoneInfo* lighting) {
    set_lighting(
        zone,
        lighting->mode_raw,
        lighting->brightness,
        lighting->red,
        lighting->green,
        lighting->blue,
        lighting->direction,
        lighting->random,
        lighting->speed
    );
}

void AsusMouseDriver::set_lighting (uint8_t zone, uint8_t mode_raw, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue, uint8_t direction, bool random, uint8_t speed) {
    if (!config.has_lighting) return;

    uint8_t req[65];
    memset(req, 0x00, sizeof(req));

    req[0x00]   = 0x00;
    req[0x01]   = 0x51;
    req[0x02]   = 0x28;
    req[0x03]   = zone;
    req[0x04]   = 0x00;
    req[0x05]   = mode_raw;
    req[0x06]   = brightness;
    req[0x07]   = red;
    req[0x08]   = green;
    req[0x09]   = blue;
    req[0x0A]   = direction;
    req[0x0B]   = random;
    req[0x0C]   = speed;

    hid_write(device, req, 65);

    await_response(req+1, 2);
}

void AsusMouseDriver::set_direct_lighting (std::vector<RGBColor>* leds, uint8_t offset) {
    if (!config.has_direct) return;

    uint8_t req[65];
    memset(req, 0x00, sizeof(req));

    uint8_t color_amount = std::min(leds->size(), (size_t) 5);

    req[0x00]   = 0x00;
    req[0x01]   = 0x51;
    req[0x02]   = 0x29;
    req[0x03]   = color_amount;
    req[0x04]   = 0x00;
    req[0x05]   = offset;
    

    for (uint8_t i = 0; i < color_amount; i++) {
        req[0x06 + i * 3] = leds->at(i).red;
        req[0x07 + i * 3] = leds->at(i).green;
        req[0x08 + i * 3] = leds->at(i).blue;
    }

    hid_write(device, req, 65);

    // no response
}

void AsusMouseDriver::set_direct_lighting (std::vector<RGBColor>* leds) {
    if (!config.has_direct) return;
    
    for (int i = config.led_count; i > 0; i -= 5) {

        uint8_t req[65];
        memset(req, 0x00, sizeof(req));

        uint8_t color_amount = std::min(i, 5);
        uint8_t offset = config.led_count - i;

        req[0x00]   = 0x00;
        req[0x01]   = 0x51;
        req[0x02]   = 0x29;
        req[0x03]   = color_amount;
        req[0x04]   = 0x00;
        req[0x05]   = offset;
        

        for (uint8_t i = 0; i < color_amount; i++) {
            req[0x06 + i * 3] = leds->at(i + offset).red;
            req[0x07 + i * 3] = leds->at(i + offset).green;
            req[0x08 + i * 3] = leds->at(i + offset).blue;
        }

        hid_write(device, req, 65);
    }
}

void AsusMouseDriver::enable_key_logging (bool enable_key_press_events, bool enable_stats) {
    uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x51;
    req[0x02]   = 0x36;
    req[0x03]   = 0x00;
    req[0x04]   = 0x00;
    req[0x05]   = (uint8_t) enable_key_press_events;
    req[0x06]   = (uint8_t) enable_stats;

    hid_write(device, req, 65);

    await_response(req+1, 2);
}

// key stats reset everytime they are requested
// distance is in dots, so you need to divide by the current dpi
// also some mice use only 16bit, so with high dpi it can overflow relatively quickly
AsusMouseDriver::KeyStats AsusMouseDriver::get_key_stats () {
    uint8_t req[65];
    memset(req, 0x00, sizeof(req));
	
    req[0x00]   = 0x00;
    req[0x01]   = 0x12;
    req[0x02]   = 0x02;

    hid_write(device, req, 65);

    std::vector<uint8_t> res = await_response(req+1, 2);

    KeyStats stats;
    stats.left_button = res[4] | res[5] << 8;
    stats.right_button = res[6] | res[7] << 8;
    stats.distace_traveled = res[8] | res[9] << 8 | res[10] << 16 | res[11] << 24;

	return stats;
}

std::vector<uint8_t> AsusMouseDriver::await_response (uint8_t* command, uint8_t command_length) {
    std::promise<std::vector<uint8_t>> prom;
    std::future future = prom.get_future();

    cb->add_packet(move(prom), command, command_length);
    std::vector<uint8_t> res = future.get();

    return res;
}
