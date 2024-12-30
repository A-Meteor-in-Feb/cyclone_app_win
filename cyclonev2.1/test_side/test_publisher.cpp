#include <iostream>
#include <thread>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"

void run_publisher_application() {

	int command_domain = 0;

	dds::domain::DomainParticipant command_participant(command_domain);

	dds::pub::Publisher command_publisher(command_participant);

	dds::topic::Topic<ControlData::tele_status> tele_status_topic(command_participant, "tele_status_data");
	dds::topic::Topic<ControlData::vehicle_status> vehicle_status_topic(command_participant, "vehicle_status_data");

	dds::pub::DataWriter<ControlData::tele_status> tele_status_writer(command_publisher, tele_status_topic);
	dds::pub::DataWriter<ControlData::vehicle_status> vehicle_status_writer(command_publisher, vehicle_status_topic);

	while (!shutdown_requested) {

		ControlData::tele_status tele_status_data("tele_1", true, true);
		ControlData::vehicle_status vehicle_status_data("vehicle_1", true, true);

		tele_status_writer.write(tele_status_data);
		vehicle_status_writer.write(vehicle_status_data);

		std::this_thread::sleep_for(std::chrono::microseconds(20));
	}
}