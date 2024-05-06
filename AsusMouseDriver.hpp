#pragma once

#include <hidapi.h>

#include <map>
#include <string>

#include "ReadCallback.hpp"

#define ASUS_ROG_CHAKRAM_X_USB_PID 								0x1A18
#define ASUS_ROG_CHAKRAM_X_2_4GHZ_PID 							0x1A1A
#define ASUS_ROG_SPATHA_X_USB_PID  								0x1977
#define ASUS_ROG_SPATHA_X_2_4GHZ_PID  							0x1979
#define ASUS_ROG_KERIS_WIRELESS_AIMPOINT_USB_PID	  			0x1A66
#define ASUS_ROG_KERIS_WIRELESS_AIMPOINT_2_4GHZ_PID  			0x1A68
#define ASUS_ROG_PUGIO_PID							  			0x1846



class AsusMouseDriver {
	public:
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


		enum LightingZones {
			LIGHTING_ZONE_LOGO = 0,
			LIGHTING_ZONE_SCROLLWHEEL = 1,
			LIGHTING_ZONE_UNDERGLOW = 2,
			LIGHTING_ZONE_ALL = 3,
			LIGHTING_ZONE_DOCK = 4,
		};

		enum LightingModes {
			LIGHTING_MODE_STATIC = 0x00,
			LIGHTING_MODE_BREATHING = 0x01,
			LIGHTING_MODE_COLOR_CYCLE = 0x02,
			LIGHTING_MODE_WAVE = 0x03,
			LIGHTING_MODE_REACTIVE = 0x04,
			LIGHTING_MODE_COMET = 0x05,
			LIGHTING_MODE_BATTERY = 0x06,
			LIGHTING_MODE_OFF = 0x07,
		};

		inline static std::string lighting_zone_names[5] {
			"Logo",
			"Scroll Wheel",
			"Underglow",
			"All",
			"Dock",
		};

		inline static std::string lighting_mode_names[8] {
			"Static",
			"Breathing",
			"Color Cycle",
			"Wave",
			"Reactive",
			"Comet",
			"Battery",
			"Off",
		};

		struct AsusMouseConfig {
			uint8_t connection_type;
			uint8_t profile_amount;
			uint8_t version_type;
			bool	is_wireless;
			bool	has_battery;
			bool	is_small_packet;

			bool	has_lighting;
			uint8_t max_brightness;
			uint8_t max_speed;
			uint8_t min_speed;
			uint8_t reasonable_max_speed;
			uint8_t reasonable_min_speed;
			uint8_t fast_speed;
			uint8_t default_speed;
			uint8_t slow_speed;

			std::map<uint8_t, LightingZones> lighting_zones;
			bool	has_dock;
			std::map<uint8_t, LightingModes> lighting_modes;
		};

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
			BatteryTimeToSleepValues time_to_sleep;
			uint8_t warning_at;
			bool is_charging;

			uint8_t unknown1;
			uint8_t unknown2;
		};

		struct LightingZoneInfo {
			bool	_is_ok;
			uint8_t mode;
			uint8_t mode_raw;
			uint8_t brightness;
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t direction;
			bool	random;
			uint8_t speed;
		};

		struct KeyStats {
			uint16_t left_button;
			uint16_t right_button;
			uint32_t distace_traveled;
		};

		AsusMouseDriver () {};
		AsusMouseDriver (std::string name, hid_device* hiddev, uint16_t pid);
		~AsusMouseDriver ();

		DeviceInfo get_device_info ();
		void set_profile (uint8_t profile);
		void save_current_profile ();
		void reset_current_profile ();
		void reset_all_profiles ();

		BatteryInfo get_battery_info ();
		bool get_wake_state();
		void set_battery_settings(BatteryTimeToSleepValues time_to_sleep, uint8_t warning_at);

		LightingZoneInfo get_lighting_zone_info (int zone);
		LightingZoneInfo get_dock_lighting ();
		std::vector<LightingZoneInfo> get_lighting_info ();

		void enable_key_logging (bool enable_key_press_events, bool enable_stats);
		KeyStats get_key_stats ();

		AsusMouseConfig 	config;
		hid_device*		device;
		std::string		name;

	private:
		ReadCallback* cb;
		std::vector<uint8_t> await_response (uint8_t* command, uint8_t command_length);
};

static std::map<uint16_t, AsusMouseDriver::AsusMouseConfig> asus_mouse_config = {
	{
		ASUS_ROG_CHAKRAM_X_2_4GHZ_PID,
		{
			AsusMouseDriver::ConnectionType::DONGLE_2_4,
			5,
			4,
			true,
			true,
			true,

			true,
			100,
			1,
			255,
			1,
			64,
			3,
			7,
			11,

			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_ZONE_LOGO },
				{ 0x01, AsusMouseDriver::LIGHTING_ZONE_SCROLLWHEEL },
				{ 0x02, AsusMouseDriver::LIGHTING_ZONE_UNDERGLOW },
			},
			false,
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_MODE_STATIC },
				{ 0x01, AsusMouseDriver::LIGHTING_MODE_BREATHING },
				{ 0x02, AsusMouseDriver::LIGHTING_MODE_COLOR_CYCLE },
				{ 0x03, AsusMouseDriver::LIGHTING_MODE_WAVE },
				{ 0x04, AsusMouseDriver::LIGHTING_MODE_REACTIVE },
				{ 0x05, AsusMouseDriver::LIGHTING_MODE_COMET },
				{ 0x06, AsusMouseDriver::LIGHTING_MODE_BATTERY },
				{ 0xF0, AsusMouseDriver::LIGHTING_MODE_OFF },
			}

		}
	},
	{
		ASUS_ROG_CHAKRAM_X_USB_PID,
		{
			AsusMouseDriver::ConnectionType::USB,
			5,
			4,
			false,
			true,
			true,

			true,
			100,
			1,
			255,
			1,
			64,
			3,
			7,
			11,
			
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_ZONE_LOGO },
				{ 0x01, AsusMouseDriver::LIGHTING_ZONE_SCROLLWHEEL },
				{ 0x02, AsusMouseDriver::LIGHTING_ZONE_UNDERGLOW },
			},
			false,
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_MODE_STATIC },
				{ 0x01, AsusMouseDriver::LIGHTING_MODE_BREATHING },
				{ 0x02, AsusMouseDriver::LIGHTING_MODE_COLOR_CYCLE },
				{ 0x03, AsusMouseDriver::LIGHTING_MODE_WAVE },
				{ 0x04, AsusMouseDriver::LIGHTING_MODE_REACTIVE },
				{ 0x05, AsusMouseDriver::LIGHTING_MODE_COMET },
				{ 0x06, AsusMouseDriver::LIGHTING_MODE_BATTERY },
				{ 0xF0, AsusMouseDriver::LIGHTING_MODE_OFF },
			}
		}
	},
	{
		ASUS_ROG_SPATHA_X_2_4GHZ_PID,
		{
			AsusMouseDriver::ConnectionType::DONGLE_2_4,
			5,
			3,
			true,
			true,
			true,

			true,
			100,
			1,
			255,
			1,
			64,
			3,
			7,
			11,
			
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_ZONE_LOGO },
				{ 0x01, AsusMouseDriver::LIGHTING_ZONE_SCROLLWHEEL },
				{ 0x02, AsusMouseDriver::LIGHTING_ZONE_UNDERGLOW },
			},
			true,
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_MODE_STATIC },
				{ 0x01, AsusMouseDriver::LIGHTING_MODE_BREATHING },
				{ 0x02, AsusMouseDriver::LIGHTING_MODE_COLOR_CYCLE },
				{ 0x03, AsusMouseDriver::LIGHTING_MODE_WAVE },
				{ 0x04, AsusMouseDriver::LIGHTING_MODE_REACTIVE },
				{ 0x05, AsusMouseDriver::LIGHTING_MODE_COMET },
				{ 0x06, AsusMouseDriver::LIGHTING_MODE_BATTERY },
				{ 0xF0, AsusMouseDriver::LIGHTING_MODE_OFF },
			}
		}
	},
	{
		ASUS_ROG_SPATHA_X_USB_PID,
		{
			AsusMouseDriver::ConnectionType::USB,
			5,
			3,
			false,
			true,
			true,

			true,
			100,
			1,
			255,
			1,
			64,
			3,
			7,
			11,
			
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_ZONE_LOGO },
				{ 0x01, AsusMouseDriver::LIGHTING_ZONE_SCROLLWHEEL },
				{ 0x02, AsusMouseDriver::LIGHTING_ZONE_UNDERGLOW },
			},
			false,
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_MODE_STATIC },
				{ 0x01, AsusMouseDriver::LIGHTING_MODE_BREATHING },
				{ 0x02, AsusMouseDriver::LIGHTING_MODE_COLOR_CYCLE },
				{ 0x03, AsusMouseDriver::LIGHTING_MODE_WAVE },
				{ 0x04, AsusMouseDriver::LIGHTING_MODE_REACTIVE },
				{ 0x05, AsusMouseDriver::LIGHTING_MODE_COMET },
				{ 0x06, AsusMouseDriver::LIGHTING_MODE_BATTERY },
				{ 0xF0, AsusMouseDriver::LIGHTING_MODE_OFF },
			}
		}
	},
	{
		ASUS_ROG_KERIS_WIRELESS_AIMPOINT_2_4GHZ_PID,
		{
			AsusMouseDriver::ConnectionType::DONGLE_2_4,
			5,
			4,
			true,
			true,
			true,

			true,
			100,
			0,
			0,
			0,
			0,
			0,
			0,
			0,

			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_ZONE_LOGO },
			},
			false,
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_MODE_STATIC },
				{ 0x01, AsusMouseDriver::LIGHTING_MODE_BREATHING },
				{ 0x02, AsusMouseDriver::LIGHTING_MODE_COLOR_CYCLE },
				{ 0x04, AsusMouseDriver::LIGHTING_MODE_REACTIVE },
				{ 0x06, AsusMouseDriver::LIGHTING_MODE_BATTERY },
				{ 0xF0, AsusMouseDriver::LIGHTING_MODE_OFF },
			}
		}
	},
	{
		ASUS_ROG_KERIS_WIRELESS_AIMPOINT_USB_PID,
		{
			AsusMouseDriver::ConnectionType::USB,
			5,
			4,
			false,
			true,
			true,

			true,
			100,
			0,
			0,
			0,
			0,
			0,
			0,
			0,

			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_ZONE_LOGO },
			},
			false,
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_MODE_STATIC },
				{ 0x01, AsusMouseDriver::LIGHTING_MODE_BREATHING },
				{ 0x02, AsusMouseDriver::LIGHTING_MODE_COLOR_CYCLE },
				{ 0x04, AsusMouseDriver::LIGHTING_MODE_REACTIVE },
				{ 0x06, AsusMouseDriver::LIGHTING_MODE_BATTERY },
				{ 0xF0, AsusMouseDriver::LIGHTING_MODE_OFF },
			}
		}
	},
	{
		ASUS_ROG_PUGIO_PID,
		{
			AsusMouseDriver::ConnectionType::USB,
			3,
			0,
			false,
			false,
			false,

			true,
			100,
			1,
			255,
			48,
			255,
			80,
			130,
			180,

			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_ZONE_LOGO },
				{ 0x01, AsusMouseDriver::LIGHTING_ZONE_SCROLLWHEEL },
				{ 0x02, AsusMouseDriver::LIGHTING_ZONE_UNDERGLOW },
			},
			false,
			{ 
				{ 0x00, AsusMouseDriver::LIGHTING_MODE_STATIC },
				{ 0x01, AsusMouseDriver::LIGHTING_MODE_BREATHING },
				{ 0x02, AsusMouseDriver::LIGHTING_MODE_COLOR_CYCLE },
				{ 0x03, AsusMouseDriver::LIGHTING_MODE_WAVE },
				{ 0x04, AsusMouseDriver::LIGHTING_MODE_REACTIVE },
				{ 0x05, AsusMouseDriver::LIGHTING_MODE_COMET },
			}
		}
	}
};
