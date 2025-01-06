#include <string>
#include <iostream>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"

bool online = true;
bool connected = false;

void update_tele_state(bool online_state, bool connected_state) {
	online = online_state;
	connected = connected_state;
}


void publisher_command_domain(int& tele_id, std::atomic<bool>& command_ato) {

	std::string tele_name = "tele" + std::to_string(tele_id);

	bool init = true;

	int command_domain = 0;

	dds::domain::DomainParticipant command_participant(command_domain);

	dds::pub::Publisher command_publisher(command_participant);

	dds::topic::Topic<ControlData::tele_status> status_topic(command_participant, "tele_status");

	dds::pub::DataWriter<ControlData::tele_status> status_writer(command_publisher, status_topic);

	while (!shutdown_requested) {

		if (!online && !connected) {
			init = true;
			command_ato = false;
		}

		if (command_ato.load()) {

			if (init) {
				init = false;
			}

			ControlData::tele_status tele_status_data(tele_name, online, connected);

			status_writer.write(tele_status_data);

		}

		if (init && !command_ato.load()) {

			ControlData::tele_status tele_status_data(tele_name, online, connected);

			status_writer.write(tele_status_data);
		}
	}
	

}