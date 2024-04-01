#pragma once

#include <hidapi.h>

#include <map>
#include <string>

struct AsusMiceConfig {
	uint8_t connection_type;
	uint8_t profile_amount;
	uint8_t version_type;
	bool	is_wireless;
	bool	has_battery;
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

		struct BatteryInfo {
			bool	_is_ok;

			uint8_t battery_charge;
			uint8_t time_to_sleep;
			uint8_t warning_at;
			bool	is_charging;

			uint8_t unknown1;
			uint8_t unknown2;
		};

		enum ConnectionType {
			USB,
			DONGLE_2_4,
			BLE,
			OMNI_2_4,
		};

		enum BatteryTimeToSleepValues {
			TIME_TO_SLEEP_ONE_MIN = 0,
			TIME_TO_SLEEP_TWO_MIN = 1,
			TIME_TO_SLEEP_THREE_MIN = 2,
			TIME_TO_SLEEP_FIVE_MIN = 3,
			TIME_TO_SLEEP_TEN_MIN = 4,
			TIME_TO_SLEEP_NEVER = 255,
		};

		// every other value corresponds to the percentage,
		// at which a warning should be given
		enum BatteryWarningValues {
			WARNING_NEVER = 0,
		};

		AsusMiceDriver () {};
		AsusMiceDriver (std::string name, hid_device* hiddev, uint16_t pid);
		~AsusMiceDriver ();

		DeviceInfo get_device_info ();
		void set_profile (uint8_t profile);

		BatteryInfo get_battery_info ();

		AsusMiceConfig 	config;
		hid_device*		device;
		std::string		name;
};

static std::map<uint16_t, AsusMiceConfig> asus_mice_config = {
	{
		ASUS_ROG_CHAKRAM_X_2_4GHZ_PID,
		{
			AsusMiceDriver::ConnectionType::DONGLE_2_4,
			5,
			4,
			true,
			true
		}
	},
	{
		ASUS_ROG_SPATHA_X_2_4GHZ_PID,
		{
			AsusMiceDriver::ConnectionType::DONGLE_2_4,
			5,
			3,
			true,
			true
		}
	},
	{
		ASUS_ROG_SPATHA_X_USB_PID,
		{
			AsusMiceDriver::ConnectionType::USB,
			5,
			3,
			false,
			true
		}
	},
	{
		ASUS_ROG_KERIS_WIRELESS_AIMPOINT_2_4GHZ_PID,
		{
			AsusMiceDriver::ConnectionType::DONGLE_2_4,
			5,
			4,
			true,
			true
		}
	}
};
