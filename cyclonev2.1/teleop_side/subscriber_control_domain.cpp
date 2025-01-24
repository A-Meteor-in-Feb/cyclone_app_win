#include <algorithm>
#include <iostream>
#include <thread>
#include <future>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"
#include "partitionName.hpp"
#include "TimeStampLogger.h"


using namespace org::eclipse::cyclonedds;

int count_recvImu = 0, count_recvSd = 0;

void subscriber_control_domain(int& tele, std::string& control_partition_name) {

	//std::string filename1 = "tele_imu.txt";
	//std::string filename2 = "tele_streamdeckButtons.txt";


	std::string name = control_partition_name;
	std::cout << "start running subscriber, partition: " << name << std::endl;
	
	std::string streamdeck_name = "streamdeck_tele" + std::to_string(tele);

	int control_domain = 1;

	dds::domain::DomainParticipant control_participant(control_domain);

	dds::sub::qos::SubscriberQos sub_qos;

	dds::core::StringSeq partition_name{ name };

	sub_qos << dds::core::policy::Partition(partition_name);

	dds::sub::Subscriber tele_subscriber(control_participant, sub_qos);

	dds::topic::Topic<ControlData::streamdeck_buttons_data> buttons_topic(control_participant, "streamdeck_buttons_data");
	dds::topic::Topic<ControlData::imu_data> imu_topic(control_participant, "imu_data");

	dds::sub::DataReader<ControlData::streamdeck_buttons_data> buttons_reader(tele_subscriber, buttons_topic);
	dds::sub::DataReader<ControlData::imu_data> imu_reader(tele_subscriber, imu_topic);

	dds::sub::LoanedSamples<ControlData::streamdeck_buttons_data> buttons_samples;
	dds::sub::LoanedSamples<ControlData::imu_data> imu_samples;

	bool connected_sd = false;
	dds::pub::qos::PublisherQos pub_qos_sd;
	dds::core::StringSeq partition_name_sd{ streamdeck_name };
	pub_qos_sd << dds::core::policy::Partition(partition_name_sd);
	dds::pub::Publisher teleSd_publisher(control_participant, pub_qos_sd);

	dds::topic::Topic<ControlData::partition_data> partiton_topic(control_participant, "partition_data");
	dds::pub::DataWriter<ControlData::partition_data> partition_writer(teleSd_publisher, partiton_topic);

	std::string timestamp;

	while (!shutdown_requested) {
	
		/*
		// RECEIVE AND TAKE THE DATA SAMPLE
		buttons_samples = buttons_reader.take();
		
		if (buttons_samples.length() > 0) {

			//std::string timestamp = TimestampLogger::getTimestamp();
			//TimestampLogger::writeToFile(filename2, timestamp);
			//std::cout << "receive buttons data from streamdeck: " << timestamp << std::endl;

			dds::sub::LoanedSamples<ControlData::streamdeck_buttons_data>::const_iterator iter;
			for (iter = buttons_samples.begin(); iter < buttons_samples.end(); ++iter) {

				const ControlData::streamdeck_buttons_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					connected_sd = true;
					//std::cout << "streamdeck_buttons_data: " << data << std::endl;
					//std::cout << "receive buttons data from streamdeck: " << timestamp << "\n" << data << "\n" << std::endl;
					count_recvSd += 1;
				}

			}
		}
		else if(!connected_sd){
			ControlData::partition_data data(name);
			partition_writer.write(data);
		}*/

		imu_samples = imu_reader.take();

		if (imu_samples.length() > 0) {

			timestamp = TimestampLogger::getTimestamp();
			//TimestampLogger::writeToFile(filename1, timestamp);
			std::cout << "receive imu data from vehicle at: " << timestamp << std::endl;

			dds::sub::LoanedSamples<ControlData::imu_data>::const_iterator iter;
			for (iter = imu_samples.begin(); iter < imu_samples.end(); ++iter) {

				const ControlData::imu_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					//std::cout << "imu_data: " << data << std::endl;
					//std::cout << "receive imu data from vehicle: " << timestamp << "\n" << data <<"\n" << std::endl;
					count_recvImu += 1;
				}

			}
		}

	}

	std::cout << "Totally received IMU messages from vehicle   : " << count_recvImu << std::endl;
	//std::cout << "Totally received Buttons data from streamdeck: " << count_recvSd << std::endl;

}

