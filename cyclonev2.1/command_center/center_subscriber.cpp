#include <algorithm>
#include <iostream>
#include <queue>
#include <typeinfo>
#include <utility>
#include <map>
#include "shutdownsignal.hpp"
#include "dds/dds.hpp"
#include "ControlData.hpp"

using namespace org::eclipse::cyclonedds;


std::vector<std::string> online_tele;
std::vector<std::string> online_vehicle;
std::map<std::string, std::string> con_te_ve;


void publish_known_msg(dds::domain::DomainParticipant& command_participant, dds::topic::Topic<ControlData::connection_msg>& con_topic, const std::string& tele_id, const std::string& vehicle_id) {

	if (tele_id == "known") {
		dds::pub::qos::PublisherQos pub_qos;

		dds::core::StringSeq partition{ vehicle_id };
		pub_qos << dds::core::policy::detail::Partition(partition);

		dds::pub::Publisher command_con_publisher(command_participant, pub_qos);

		dds::core::QosProvider provider("ReliableQos.xml");
		auto writer_qos = provider.datawriter_qos("myqos::reliable_writer");

		dds::pub::DataWriter con_topic_writer(command_con_publisher, con_topic, writer_qos);

		ControlData::connection_msg con_msg(tele_id, vehicle_id);

		con_topic_writer.write(con_msg);
	}

	if (vehicle_id == "known") {

		dds::pub::qos::PublisherQos pub_qos;

		dds::core::StringSeq partition{ tele_id };
		pub_qos << dds::core::policy::detail::Partition(partition);

		dds::pub::Publisher command_con_publisher(command_participant, pub_qos);

		dds::core::QosProvider provider("ReliableQos.xml");
		auto writer_qos = provider.datawriter_qos("myqos::reliable_writer");

		dds::pub::DataWriter con_topic_writer(command_con_publisher, con_topic, writer_qos);

		ControlData::connection_msg con_msg(tele_id, vehicle_id);

		con_topic_writer.write(con_msg);

	}
}

//publish the connection message in strictly reliable way...
//Also, publisher will only publish the connection msg to the relevant subscribers,
//so, we need set Partition Qos, then do the initialization of Data Writer.
// ----
// HOWEVER, Partition Qos can only be applied to publisher (subscriber) ......
// ----
//Use the QoS settings.
void publish_connection_msg(dds::domain::DomainParticipant& command_participant, dds::topic::Topic<ControlData::connection_msg>& con_topic, const std::string& tele_id, const std::string& vehicle_id, bool tele_again, bool vehicle_again) {
	
	if (!tele_again && !vehicle_again) {
		dds::pub::qos::PublisherQos pub_qos;

		dds::core::StringSeq partition{ tele_id, vehicle_id };
		pub_qos << dds::core::policy::detail::Partition(partition);

		dds::pub::Publisher command_con_publisher(command_participant, pub_qos);

		dds::core::QosProvider provider("ReliableQos.xml");
		auto writer_qos = provider.datawriter_qos("myqos::reliable_writer");

		dds::pub::DataWriter con_topic_writer(command_con_publisher, con_topic, writer_qos);

		ControlData::connection_msg con_msg(tele_id, vehicle_id);

		con_topic_writer.write(con_msg);
	}
	else if (!tele_again && vehicle_again) {
		dds::pub::qos::PublisherQos pub_qos;

		dds::core::StringSeq partition{ vehicle_id };
		pub_qos << dds::core::policy::detail::Partition(partition);

		dds::pub::Publisher command_con_publisher(command_participant, pub_qos);

		dds::core::QosProvider provider("ReliableQos.xml");
		auto writer_qos = provider.datawriter_qos("myqos::reliable_writer");

		dds::pub::DataWriter con_topic_writer(command_con_publisher, con_topic, writer_qos);

		ControlData::connection_msg con_msg(tele_id, vehicle_id);

		con_topic_writer.write(con_msg);
	}
	else if (tele_again && !vehicle_again) {
		dds::pub::qos::PublisherQos pub_qos;

		dds::core::StringSeq partition{ tele_id };
		pub_qos << dds::core::policy::detail::Partition(partition);

		dds::pub::Publisher command_con_publisher(command_participant, pub_qos);

		dds::core::QosProvider provider("ReliableQos.xml");
		auto writer_qos = provider.datawriter_qos("myqos::reliable_writer");

		dds::pub::DataWriter con_topic_writer(command_con_publisher, con_topic, writer_qos);

		ControlData::connection_msg con_msg(tele_id, vehicle_id);

		con_topic_writer.write(con_msg);
	}
	

	/*if (tele_id == "non-matched") {
		dds::core::StringSeq partition{ vehicle_id };
		pub_qos << dds::core::policy::detail::Partition(partition);
	}
	else if (vehicle_id == "non-matched") {
		dds::core::StringSeq partition{ tele_id };
		pub_qos << dds::core::policy::detail::Partition(partition);
	}
	else {
		dds::core::StringSeq partition{ tele_id, vehicle_id };
		pub_qos << dds::core::policy::detail::Partition(partition);
	}*/

	
}



//add the potential pairs into the tele_vehicle pair.
void judge_connection(dds::domain::DomainParticipant& command_participant, dds::topic::Topic<ControlData::connection_msg>& con_topic) {

	int tele_num = online_tele.size();
	int vehicle_num = online_vehicle.size();

	while (tele_num && vehicle_num) {

		std::string usable_vehicle = online_vehicle.back();
		std::string usable_tele = online_tele.back();

		online_vehicle.pop_back();
		online_tele.pop_back();

		tele_num -= 1;
		vehicle_num -= 1;

		con_te_ve.insert(std::make_pair(usable_tele, usable_vehicle));
		con_te_ve.insert(std::make_pair(usable_vehicle, usable_tele));

		publish_connection_msg(command_participant, con_topic, usable_tele, usable_vehicle, false, false);
	}


	/*
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

			publish_connection_msg(command_participant, con_topic, usable_tele, usable_vehicle);
		}

		while (tele_num) {
			std::string usable_tele = online_tele.back();
			std::string usable_vehicle = "non-matched";
			tele_num -= 1;

			publish_connection_msg(command_participant, con_topic, usable_tele, usable_vehicle);
		}

		while (vehicle_num) {
			std::string usable_vehicle = online_vehicle.back();
			std::string usable_tele = "non-matched";
			vehicle_num -= 1;

			publish_connection_msg(command_participant, con_topic, usable_tele, usable_vehicle);
		}
		
	}*/

	
}


void run_subscriber_application() {

	int command_domain = 0;

	dds::domain::DomainParticipant command_participant(command_domain);


	//---- initialize publisher and data writer and related topics
	dds::topic::Topic<ControlData::connection_msg> con_topic(command_participant, "connection_msg");
	dds::topic::Topic<ControlData::disconnection_msg> discon_topic(command_participant, "disconnection_msg");


	//---- initializa subscriber and data readers and related topics
	dds::sub::Subscriber command_subscriber(command_participant);

	dds::topic::Topic<ControlData::tele_status> tele_status_topic(command_participant, "tele_status");
	dds::topic::Topic<ControlData::vehicle_status> vehicle_status_topic(command_participant, "vehicle_status");
	
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

					std::cout << "\ntele_status data:    " << data << std::endl;

					std::string tele_id = data.tele_id();

					std::string tele_state = data.connected() ? "conneccted" : "online";


					if (tele_state == "online" && con_te_ve.count(tele_id) == 0) {
						bool usable = true;

						for (int i = 0; i < online_tele.size(); i++) {
							if (online_tele.at(i) == tele_id) {
								usable = false;
								break;
							}
						}

						if (usable) {
							publish_known_msg(command_participant, con_topic, tele_id, "known");
							online_tele.push_back(tele_id);
						}
					}
					else if (tele_state == "online" && con_te_ve.count(tele_id)) {
						publish_connection_msg(command_participant, con_topic, tele_id, con_te_ve[tele_id], true, false);
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

					std::cout << "\nvehicle_status data: " << data << std::endl;

					std::string vehicle_id = data.vehicle_id();
					
					std::string vehicle_state = data.connected() ? "connected" : "online";


					if (vehicle_state == "online" && con_te_ve.count(vehicle_id) == 0) {
						bool usable = true;

						for (int i = 0; i < online_vehicle.size(); i++) {
							if (online_vehicle.at(i) == vehicle_id) {
								usable = false;
								break;
							}
						}

						if (usable) {
							publish_known_msg(command_participant, con_topic, "known", vehicle_id);
							online_vehicle.push_back(vehicle_id);
						}
					}
					else if (vehicle_state == "online" && con_te_ve.count(vehicle_id)) {
						publish_connection_msg(command_participant, con_topic, con_te_ve[vehicle_id], vehicle_id, false, true);
					}

					
				}

			}
		}

		if (online_tele.size() > 0 && online_vehicle.size() > 0) {
			judge_connection(command_participant, con_topic);
		}

	}
}

/*

					//This place, we need to fixxxxxxxxx (x_x) (>_<) (=_=')
					if (tele_state == "offline" && con_te_ve.count(tele_id)) {

						std::string discon_tele = tele_id;
						std::string discon_vehicle = con_te_ve[tele_id];

						//send dis-connection msg. TO THE SPECIFIC TELE !!!
						dds::pub::qos::PublisherQos pub_qos;
						dds::core::StringSeq partition{ discon_tele, discon_vehicle };
						pub_qos << dds::core::policy::detail::Partition(partition);
						dds::pub::Publisher command_discon_publisher(command_participant, pub_qos);
						dds::pub::DataWriter<ControlData::disconnection_msg> discon_writer(command_discon_publisher, discon_topic);

						ControlData::disconnection_msg discon_msg("disconnection");
						discon_writer.write(discon_msg);

						//delete tele_id:vehicle_id && vehicle_id:tele_id pairs.
						con_te_ve.erase(discon_tele);
						con_te_ve.erase(discon_vehicle);
					}

*/

/*
* //This place, we need to fixxxxxxxxx (x_x) (>_<) (=_=')
					if (vehicle_state == "offline" && con_te_ve.count(vehicle_id)) {

						std::string discon_vehicle = vehicle_id;
						std::string discon_tele = con_te_ve[vehicle_id];

						//send dis-connection msg. TO THE SPECIFIC VEHICLE !!!!!!
						dds::pub::qos::PublisherQos pub_qos;
						dds::core::StringSeq partition{ discon_tele, discon_vehicle };
						pub_qos << dds::core::policy::detail::Partition(partition);
						dds::pub::Publisher command_discon_publisher(command_participant, pub_qos);
						dds::pub::DataWriter<ControlData::disconnection_msg> discon_writer(command_discon_publisher, discon_topic);

						ControlData::disconnection_msg discon_msg("disconnection");
						discon_writer.write(discon_msg);
						

						//delete tele_id:vehicle_id && vehicle_id : tele_id pairs.
						con_te_ve.erase(discon_vehicle);
						con_te_ve.erase(discon_tele);
					}
*/