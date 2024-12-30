#include <algorithm>
#include <iostream>
#include <map>

#include <typeinfo>

#include "shutdownsignal.hpp"
#include "dds/dds.hpp"
#include "ControlData.hpp"

using namespace org::eclipse::cyclonedds;

std::map<std::string, std::string> vehicle_map;
std::map<std::string, std::string> tele_map;
std::map<std::string, std::string> tele_vehicle_pair;

//add the potential pairs into the tele_vehicle_pair.
void judge_connection(std::atomic<bool>& p_conn) {

	bool vehicle_usable = false;
	bool tele_usable = false;

	std::map<std::string, std::string> ::iterator vehicle_iter;
	std::map<std::string, std::string> ::iterator tele_iter;
	std::map<std::string, std::string> ::iterator tele_vehicle_iter;

	for (tele_iter = tele_map.begin(); tele_iter != tele_map.end(); tele_iter++) {

		if (tele_iter->second == "online") {

			for (vehicle_iter = vehicle_map.begin(); vehicle_iter != vehicle_map.end(); vehicle_iter++) {

				if (vehicle_iter->second == "online") {

				}
			}
		}
	}

	

}

void run_subscriber_application(std::atomic<bool>& p_conn) {

	int command_domain = 0;

	dds::domain::DomainParticipant command_participant(command_domain);
	dds::sub::Subscriber command_subscriber(command_participant);

	dds::topic::Topic<ControlData::tele_status> tele_status_topic(command_participant, "tele_status_data");
	dds::topic::Topic<ControlData::vehicle_status> vehicle_status_topic(command_participant, "vehicle_status_data");
	
	dds::sub::DataReader<ControlData::tele_status> tele_status_reader(command_subscriber, tele_status_topic);
	dds::sub::DataReader<ControlData::vehicle_status> vehicle_status_reader(command_subscriber, vehicle_status_topic);
	
	dds::sub::LoanedSamples<ControlData::tele_status> tele_status_samples;
	dds::sub::LoanedSamples<ControlData::vehicle_status> vehicle_status_samples;
	

	while (!shutdown_requested) {

		tele_status_samples = tele_status_reader.take();
		if (tele_status_samples.length() > 0) {

			dds::sub::LoanedSamples<ControlData::tele_status>::const_iterator iter;
			for (iter = tele_status_samples.begin(); iter < tele_status_samples.end(); ++iter) {

				const ControlData::tele_status& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {

					std::cout << "tele_status data: " << data << std::endl;

					std::string tele_id = data.tele_id();

					if (tele_vehicle_pair.count(tele_id)) {

						std::string tele_state = "connected";
						
						if (!data.connected()) {

							if (data.online()) {

								std::string vehicle = tele_vehicle_pair[tele_id];

								if (vehicle_map[vehicle] != "offline" ) {
									//transmit the connection msg again.
								}
								else {
									//delete this pair, set tele_state = "online"
									//transmit the disconnection msg.
								}
							}
							else {
								//judge vehicle's state, unless it's offline, make it be online.
								//delete the pair.
								//transmit the disconnection msg.
							}
						}
						

					}else {

						std::string tele_state = "offline";

						if (data.online()) {
							tele_state = "online";
							if (tele_map.count(tele_id) == 0) {
								tele_map.insert(std::make_pair(tele_id, tele_state));
							}
						}
						else {
							if (tele_map.count(tele_id) != 0) {
								tele_map.erase(tele_id);
							}
						}

						
					}

				}

			}
		}

		vehicle_status_samples = vehicle_status_reader.take();
		if (vehicle_status_samples.length() > 0) {

			dds::sub::LoanedSamples<ControlData::vehicle_status>::const_iterator iter;
			for (iter = vehicle_status_samples.begin(); iter < vehicle_status_samples.end(); ++iter) {

				const ControlData::vehicle_status& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {

					std::cout << "vehicle_status data: " << data << std::endl;

					std::string vehicle_id = data.vehicle_id();
					std::string vehicle_state = "offline";

					//judge the state and add this vehicle to the corresponding map.
					if (data.connected()) {
						vehicle_state = "connected";
					}
					else if (data.online()) {
						vehicle_state = "online";
					}

					if (vehicle_map.count(vehicle_id)) {
						vehicle_map[vehicle_id] = vehicle_state;
					}
					else {
						vehicle_map.insert(std::make_pair(vehicle_id, vehicle_state));
					}
				}

			}
		}



	}
}