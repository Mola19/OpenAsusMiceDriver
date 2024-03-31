#include <string>
#include <optional>
#include <vector>
#include <hidapi.h>

struct HIDDetectionInformation {
	std::string				name;
	uint16_t				vid;
	uint16_t				pid;
	int32_t					interface;
	std::optional<uint16_t>	usage_page;
	std::optional<uint16_t>	usage;
	bool compare(hid_device_info* dev_info) {
		return (
			vid == dev_info->vendor_id
			&& pid == dev_info->product_id
			&& interface == dev_info->interface_number
			&& (!usage_page || usage_page == dev_info->usage_page)
			&& (!usage || usage == dev_info->usage)
			);
	}
};

static std::vector<HIDDetectionInformation> hid_devices {
	{
		"Asus ROG Chakram X 2.4 GHz dongle",
		0x0B05,
		0x1A1A,
		0x0000,
		0xFF01,
		0x0001
	},
	{
		"Asus ROG Spatha X 2.4 GHz dock",
		0x0B05,
		0x1979,
		0x0000,
		0xFF01,
		0x0001
	},
	{
		"Asus ROG Spatha X USB",
		0x0B05,
		0x1977,
		0x0000,
		0xFF01,
		0x0001
	},
	{
		"Asus ROG Keris Wireless AimPoint 2.4 GHz dongle",
		0x0B05,
		0x1A68,
		0x0000,
		0xFF01,
		0x0001
	}
};