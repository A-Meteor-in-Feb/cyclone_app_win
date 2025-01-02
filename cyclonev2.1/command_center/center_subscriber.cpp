#include <algorithm>
#include <iostream>
#include <queue>
#include <typeinfo>
#include <utility>
#include <map>
#include <TCorePolicy.hpp>
#include "shutdownsignal.hpp"
#include "dds/dds.hpp"
#include "ControlData.hpp"

using namespace org::eclipse::cyclonedds;


std::vector<std::string> online_tele;
std::vector<std::string> online_vehicle;
std::map<std::string, std::string> con_te_ve;


//publish the connection message in strictly reliable way...
//Also, publisher will only publish the connection msg to the relevant subscribers,
//so, we need set Partition Qos, then do the initialization of Data Writer.
//Use the QoS settings.
void run_publisher_application(dds::pub::Publisher& command_publisher, dds::topic::Topic<ControlData::connection_msg>& con_topic, const std::string& tele_id, const std::string& vehicle_id) {

	dds::pub::qos::DataWriterQos w_qos;
	w_qos << dds::core::policy::detail::Reliability(dds::core::policy::ReliabilityKind_def::RELIABLE);
	w_qos << dds::core::policy::detail::History(dds::core::policy::HistoryKind_def::KEEP_ALL);

	if (tele_id == "non-mateched") {
		w_qos << dds::core::policy::detail::Partition(dds::core::policy::detail::TPartition({ vehicle_id }));
	}
	else if (vehicle_id = "non-matched") {
		w_qos << dds::core::policy::detail::Partition(dds::core::policy::detail::TPartition({ tele_id }));
	}
	else {
		w_qos << dds::core::policy::detail::Partition(dds::core::policy::detail::TPartition({ tele_id, vehicle_id }));
	}
	
	dds::pub::DataWriter con_topic_writer(command_publisher, con_topic, w_qos);

	ControlData::connection_msg con_msg(tele_id, vehicle_id);
	con_topic_writer.write(con_msg);
}



//add the potential pairs into the tele_vehicle pair.
void judge_connection(dds::pub::Publisher& command_publisher, dds::topic::Topic<ControlData::connection_msg>& con_topic) {

	if (!online_tele.empty() || !online_vehicle.empty()) {

		int tele_num = online_tele.size();
		int vehicle_num = online_vehicle.size();

		while ( tele_num && vehicle_num) {

			std::string usable_vehicle = online_vehicle.back();
			std::string usable_tele = online_tele.back();

			online_vehicle.pop_back();
			online_tele.pop_back();

			tele_num -= 1;
			vehicle_num -= 1;

			con_te_ve.insert(std::make_pair(usable_tele, usable_vehicle));
			con_te_ve.insert(std::make_pair(usable_vehicle, usable_tele));

			run_publisher_application(command_publisher, con_topic, usable_tele, usable_vehicle);
		}

		while (tele_num) {
			std::string usable_tele = online_tele.back();
			std::string usable_vehicle = "non-matched";
			tele_num -= 1;

			run_publisher_application(command_publisher, con_topic, usable_tele, usable_vehicle);
		}

		while (vehicle_num) {
			std::string usable_vehicle = online_vehicle.back();
			std::string usable_tele = "non-matched";
			vehicle_num -= 1;

			run_publisher_application(command_publisher, con_topic, usable_tele, usable_vehicle);
		}
		
	}

	
}


void run_subscriber_application(std::atomic<bool>& p_conn) {

	int command_domain = 0;

	dds::domain::DomainParticipant command_participant(command_domain);


	//---- initialize publisher and data writer and related topics
	dds::pub::Publisher command_publisher(command_participant);

	dds::topic::Topic<ControlData::connection_msg> con_topic(command_participant, "connection_msg");


	//---- initializa subscriber and data readers and related topics
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
					std::string tele_state = data.connected() && data.online() ? "connected" : "online";
					tele_state = data.online() && !data.connected() ? "online" : "offline";

					if (tele_state == "online") {
						bool usable = true;

						for (int i = 0; i < online_tele.size(); i++) {
							if (online_tele.at(i) == tele_id) {
								usable = false;
								break;
							}
						}

						if (usable) {
							online_tele.push_back(tele_id);
						}
					}

					if (tele_state == "offline" && con_te_ve.count(tele_id)) {
						//send dis-connection msg. 
						//delete tele_id:vehicle_id && vehicle_id:tele_id pairs.
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
					std::string vehicle_state = data.online() && data.connected() ? "connected" : "online";
					vehicle_state = data.online && !data.connected() ? "online" : "offline";

					if (vehicle_state == "online") {
						bool usable = true;

						for (int i = 0; i < online_vehicle.size(); i++) {
							if (online_vehicle.at(i) == vehicle_id) {
								usable = false;
								break;
							}
						}

						if (usable) {
							online_vehicle.push_back(vehicle_id);
						}
					}

					if (vehicle_state == "offline" && con_te_ve.count(vehicle_id)) {
						//send dis-connection msg. 
						//delete tele_id:vehicle_id && vehicle_id : tele_id pairs.
					}
				}

			}
		}

		judge_connection(command_publisher, con_topic);

	}
}