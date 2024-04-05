#pragma once

#include <vector>
#include <future>
#include <thread>

#include <hidapi.h>

class ReadCallback {
	public:
		ReadCallback (hid_device* dev);
		~ReadCallback ();

		void add_packet (std::promise<std::vector<uint8_t>>&& prom, uint8_t* command, uint8_t command_length);

	private:
		struct Packet {
			Packet (std::promise<std::vector<uint8_t>>&& prom, uint8_t* command, uint8_t command_length) {
				this->prom = move(prom);
				this->command = command;
				this->command_length = command_length;
			}
			
			uint8_t* command;
			uint8_t command_length;
			std::promise<std::vector<uint8_t>> prom;
		};

		hid_device* hiddev;
		std::vector<Packet*>* packets;
		std::thread* read_thread;

		bool kill_read_thread = false;

		void read_thread_fn ();
};