#include <string>
#include <iostream>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"
#include "partitionName.hpp"
#include "TimeStampLogger.h"



void publisher_control_domain(int& tele, std::string& partition_name);
void subscriber_control_domain(int& tele, std::string& partition_name);


int count_ConMsg = 0;


void run_command_domain(int& tele) {

	//const std::string filename = "tele_connection_msg.txt";

	std::string tele_id = "tele" + std::to_string(tele);
	bool online_state = true;
	bool connected_state = false;

	partitionName control_partition_name;

	int command_domain = 0;

	dds::domain::DomainParticipant command_participant(command_domain);

	// ====== PUBLISHER ======
	dds::pub::Publisher command_publisher(command_participant);
	dds::topic::Topic<ControlData::tele_status> status_topic(command_participant, "tele_status");
	dds::pub::DataWriter<ControlData::tele_status> status_writer(command_publisher, status_topic);

	// ====== SUBSCRIBER ======
	dds::sub::qos::SubscriberQos sub_qos;

	dds::core::StringSeq partition_name{ tele_id };

	sub_qos << dds::core::policy::Partition(partition_name);

	dds::sub::Subscriber command_subscriber(command_participant, sub_qos);

	dds::topic::Topic<ControlData::connection_msg> con_topic(command_participant, "connection_msg");
	dds::topic::Topic<ControlData::disconnection_msg> discon_topic(command_participant, "disconnection_msg");

	dds::core::QosProvider provider("ReliableQos.xml");
	auto reader_qos = provider.datareader_qos("myqos::reliable_reader");

	dds::sub::DataReader<ControlData::connection_msg> con_reader(command_subscriber, con_topic, reader_qos);
	dds::sub::DataReader<ControlData::disconnection_msg> discon_reader(command_subscriber, discon_topic, reader_qos);

	dds::sub::LoanedSamples<ControlData::connection_msg> con_samples;
	dds::sub::LoanedSamples<ControlData::disconnection_msg> discon_samples;

	bool known = false;
	std::string name;
	std::string timestamp;

	while (!shutdown_requested) {

		con_samples = con_reader.take();

		if (con_samples.length() > 0) {

			timestamp = TimestampLogger::getTimestamp();

			//TimestampLogger::writeToFile(filename, timestamp);
			std::cout << "receive connection msg at: " << timestamp << std::endl;


			dds::sub::LoanedSamples<ControlData::connection_msg>::const_iterator iter;

			for (iter = con_samples.begin(); iter < con_samples.end(); ++iter) {

				const ControlData::connection_msg& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {

					count_ConMsg += 1;

					std::string vehicle_id = data.vehicle_id();

					if (vehicle_id == "known") {

						known = true;
						std::cout << "non-matched vehicle for now ..." << std::endl;

					}
					else {
						std::cout << "match to vehicle: " << vehicle_id << std::endl;

						//send the latest state .
						online_state = true;
						connected_state = true;
						ControlData::tele_status tele_status_data(tele_id, online_state, connected_state);
						status_writer.write(tele_status_data);

						//start to run the functions of control domain
						//set the partition name
						//get the vehicle's ip address.
						name = data.tele_id() + data.vehicle_id();
						std::cout << "partition name: " << name << std::endl;

						std::thread tele_control_publisher(publisher_control_domain, std::ref(tele), std::ref(name));
						std::thread tele_control_subscriber(subscriber_control_domain, std::ref(tele), std::ref(name));

						tele_control_publisher.join();
						tele_control_subscriber.join();
						
					}


				}
			}
		} // This situation means the command center did not receive any status info, 
		  // and we need to keep writing until the commend center side has recorded the info about the teleop.
		else if (!known) {
			ControlData::tele_status tele_status_data(tele_id, online_state, connected_state);
			status_writer.write(tele_status_data);
			timestamp = TimestampLogger::getTimestamp();
			std::cout << "publish status msg at: " << timestamp << std::endl;
			std::this_thread::sleep_for(std::chrono::microseconds(20));
		}

		/*
		* Here we need to figure out what kind of conditions will trigger this ...... 
		
		discon_samples = discon_reader.take();

		if (discon_samples.length() > 0) {
			dds::sub::LoanedSamples<ControlData::disconnection_msg>::const_iterator iter;

			for (iter = discon_samples.begin(); iter < discon_samples.end(); ++iter) {

				const ControlData::disconnection_msg& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {

					std::cout << "Received Disconnection Msg: " << data.msg() << std::endl;

					online_state = false;
					connected_state = false;
					ControlData::tele_status tele_status_data(tele_name, online_state, connected_state);
					status_writer.write(tele_status_data);

					control_partition_name.setPartitionName("none");

				}
			}
		}*/
	}


	std::cout << "preparing shutdown ..." << std::endl;
	std::cout << "Totally received connection msg from the command center: " << count_ConMsg << std::endl;
	std::cout << "From teleop side, totally 200 controller messages are sent." << std::endl;
}



int main(int argc, char* argv[]) {

	int tele = -1;

	if (argc > 2 && strcmp(argv[1], "-id") == 0) {
		tele = atoi(argv[2]);
	}


	try {
		run_command_domain(std::ref(tele));
	}
	catch (const std::exception& ex) {
		// This will catch DDS exceptions
		std::cerr << "Exception in run_publisher_application(): " << ex.what()
			<< std::endl;
		return EXIT_FAILURE;
	}
	catch (...) {
		std::cerr << "Unknown Error :(" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}