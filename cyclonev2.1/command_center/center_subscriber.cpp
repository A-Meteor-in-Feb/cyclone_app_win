#include <algorithm>
#include <iostream>
#include <map>

#include "shutdownsignal.hpp"
#include "dds/dds.hpp"
#include "CommandData.hpp"

using namespace org::eclipse::cyclonedds;

void run_subscriber_application(std::atomic<bool>& p_conn) {

	int command_domain = 0;

	dds::domain::DomainParticipant command_participant(command_domain);
	dds::sub::Subscriber command_subscriber(command_participant);

	dds::topic::Topic<CommandData::tele_status> tele_ststus_topic(command_participant, "tele_status_data");
	dds::topic::Topic<CommandData::vehicle_status> vehicle_status_topic(command_particiant, "vehicle_status_data");
	dds::topic::Topic<CommandData::vehicle_gps> vehicle_gps_topic(command_participant, "vehicle_gps_data");

	dds::sub::DataReader<CommandData::tele_status> tele_status_reader(command_subscriber, tele_status_topic);
	dds::sub::DataReader<CommandData::vehicle_status> vehicle_status_reader(command_subscriber, vehicle_status_topic);
	dds::sub::DataReader<CommandData::vehicle_gps> vehicle_gps_reader(command_subscriber, vehicle_gps_topic);

	dds::sub::LoanedSamples<CommandData::tele_status> tele_status_samples;
	dds::sub::LoanedSamples<CommandData::vehicle_status> vehicle_status_samples;
	dds::sub::LoanedSamples<CommandData::vehicle_gps> vehicle_gps_samples;

	while (!shutdown_requested) {

		tele_status_samples = tele_status_reader.take();
		if (tele_status_samples.length() > 0) {
			// how to deal with this data???
		}

		vehicle_status_samples = vehicle_status_reader.take();
		if (vehicle_status_samples.length() > 0) {
			// how to deal with this data???
		}

		vehicle_gps_samples = vehicle_gps_reader.take();
		if (vehicle_gps_samples.length() > 0) {
			//how to deal with this data???
		}
	}
}