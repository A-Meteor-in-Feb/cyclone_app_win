#include <algorithm>
#include <iostream>
#include <map>

#include "shutdownsignal.hpp"
#include "dds/dds.hpp"
#include "CommandData.hpp"

using namespace org::eclipse::cyclonedds;

std::map<std::string, std::string> vehile_map;
std::map<std::string, std::string> tele_map;
std::map<std::string, std::vector<double>> vehicle_location_map;
std::map<std::string, std::string> tele_vehicle_pair;

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

			dds::sub::LoanedSamples<CommandData::tele_status>::const_iterator iter;
			for (iter = tele_status_samples.begin(); iter < tele_status_samples.end(); ++iter) {

				const TeleData::tele_status& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "tele_status data: " << data << std::endl;

					std::string tele_state;

					//Can we do this to get the value????
					std::string tele_name = data::tele_status()::tele_id();
					bool tele_online = data::tele_status()::online();
					bool tele_connected = data::tale_status()::connected();

					if (tele_connected) {
						tele_state = "connected";
					}
					else {
						tele_state = "online";
					}


					if (tele_map.count[tele_name]) {
						tele_map[tele_name] = tele_state;
					}
					else {
						tele_map.insert(std::make_pair(tele_name, tele_state));
					}

				}
			}
		}

		vehicle_status_samples = vehicle_status_reader.take();
		if (vehicle_status_samples.length() > 0) {

			dds::sub::LoanedSamples<CommandData::vehicle_status>::const_iterator iter;
			for (iter = vehicle_status_samples.begin(); iter < vehicle_status_samples.end(); ++iter) {


			}
		}

		vehicle_gps_samples = vehicle_gps_reader.take();
		if (vehicle_gps_samples.length() > 0) {
			//how to deal with this data???
		}
	}
}