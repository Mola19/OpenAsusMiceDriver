#pragma once

#include <hidapi.h>

#include <map>
#include <string>

struct AsusMiceConfig {
	uint8_t connection_type;
	uint8_t profile_amount;
	uint8_t version_type;
	bool	is_wireless;
};

#define ASUS_ROG_CHAKRAM_X_2_4GHZ_PID 							0x1A1A
#define ASUS_ROG_SPATHA_X_USB_PID  								0x1977
#define ASUS_ROG_SPATHA_X_2_4GHZ_PID  							0x1979
#define ASUS_ROG_KERIS_WIRELESS_AIMPOINT_2_4GHZ_PID  			0x1A68



class AsusMiceDriver {
	public: 
		struct DeviceInfo {
			std::string version;
			std::string dongle_version;
			uint8_t 	profile_size;
			uint8_t		mode_size;
			uint8_t		active_profile;
			uint8_t		active_dpi_index;
			uint16_t	active_dpi;
		};

		enum ConnectionType {
			USB,
			DONGLE_2_4,
			BLE,
			OMNI_2_4,
		};

		AsusMiceDriver () {};
		AsusMiceDriver (std::string name, hid_device* hiddev, uint16_t pid);
		~AsusMiceDriver ();

		DeviceInfo get_device_info ();
		void set_profile (uint8_t profile);

		AsusMiceConfig 	config;
		hid_device*		device;
		std::string		name;
	private:
		void send_data (uint8_t* bytes, size_t size);
		void read_data (uint8_t* bytes, size_t size);
};

static std::map<uint16_t, AsusMiceConfig> asus_mice_config = {
	{
		ASUS_ROG_CHAKRAM_X_2_4GHZ_PID,
		{
			AsusMiceDriver::ConnectionType::DONGLE_2_4,
			5,
			4,
			true
		}
	},
	{
		ASUS_ROG_SPATHA_X_2_4GHZ_PID,
		{
			AsusMiceDriver::ConnectionType::DONGLE_2_4,
			5,
			3,
			true
		}
	},
	{
		ASUS_ROG_SPATHA_X_USB_PID,
		{
			AsusMiceDriver::ConnectionType::USB,
			5,
			3,
			false
		}
	},
	{
		ASUS_ROG_KERIS_WIRELESS_AIMPOINT_2_4GHZ_PID,
		{
			AsusMiceDriver::ConnectionType::DONGLE_2_4,
			5,
			4,
			true
		}
	}
};
