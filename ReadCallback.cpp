#include <vector>
#include <future>
#include <iostream>
#include <string>
#include <cstring>

#include "ReadCallback.hpp"

ReadCallback::ReadCallback (hid_device* dev) {
	packets = new std::vector<Packet*>();
	hiddev = dev;

	read_thread = new std::thread(&ReadCallback::read_thread_fn, this);
}

ReadCallback::~ReadCallback () {
	kill_read_thread = true;
	read_thread->join();
}

void ReadCallback::read_thread_fn () {
	while (!kill_read_thread) {
		uint8_t res[64];
		memset(res, 0x00, 64);
    	int ret = hid_read_timeout(hiddev, res, 64, 100);

		if (ret < 1) continue;

		if (res[0] == 0xFF && res[1] == 0xAA) {
			// todo return first element as error
			if (packets->size() == 0) continue;
			std::vector res_vec (res, res + 64);
			packets->at(0)->prom.set_value(res_vec);
			packets->erase(packets->begin());
			continue;
		}

		uint64_t i = 0;
		for (Packet* packet : *packets) {
			bool failed = false;
			for (uint8_t j = 0; j < packet->command_length; j++) {
				if (res[j] != packet->command[j]) failed = true;
			}

			if (!failed) {
				std::vector res_vec (res, res + 64);
				packet->prom.set_value(res_vec);
				packets->erase(packets->begin() + i);
			}
			i++;
		}
	}
}

void ReadCallback::add_packet (std::promise<std::vector<uint8_t>>&& prom, uint8_t* command, uint8_t command_length) {
	Packet* packet = new Packet(move(prom), command, command_length);
	packets->push_back(packet);
}
