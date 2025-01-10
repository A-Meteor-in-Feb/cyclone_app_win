#include <algorithm>
#include <iostream>
#include <thread>
#include <future>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"
#include "partitionName.hpp"

using namespace org::eclipse::cyclonedds;


void subscriber_control_domain(int& tele_id, std::string& control_partition_name) {

	std::string name = control_partition_name;

	std::cout << "start running subscriber, partition: " << name << std::endl;

	int control_domain = 1;

	dds::domain::DomainParticipant control_participant(control_domain);

	dds::sub::qos::SubscriberQos sub_qos;

	dds::core::StringSeq partition_name{ name };

	sub_qos << dds::core::policy::Partition(partition_name);

	dds::sub::Subscriber tele_subscriber(control_participant, sub_qos);
	//dds::sub::Subscriber tele_subscriber1(control_participant);

	//dds::topic::Topic<ControlData::streamdeck_buttons_data> buttons_topic(control_participant, "streamdeck_buttons_data");
	dds::topic::Topic<ControlData::imu_data> imu_topic(control_participant, "imu_data");

	//dds::sub::DataReader<ControlData::streamdeck_buttons_data> buttons_reader(tele_subscriber1, buttons_topic);
	dds::sub::DataReader<ControlData::imu_data> imu_reader(tele_subscriber, imu_topic);

	//dds::sub::LoanedSamples<ControlData::streamdeck_buttons_data> buttons_samples;
	dds::sub::LoanedSamples<ControlData::imu_data> imu_samples;

	while (!shutdown_requested) {
		/*
		// RECEIVE AND TAKE THE DATA SAMPLE
		buttons_samples = buttons_reader.take();

		if (buttons_samples.length() > 0) {

			dds::sub::LoanedSamples<ControlData::streamdeck_buttons_data>::const_iterator iter;
			for (iter = buttons_samples.begin(); iter < buttons_samples.end(); ++iter) {

				const ControlData::streamdeck_buttons_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "streamdeck_buttons_data: " << data << std::endl;

				}

			}
		}*/

		imu_samples = imu_reader.take();

		if (imu_samples.length() > 0) {
			dds::sub::LoanedSamples<ControlData::imu_data>::const_iterator iter;
			for (iter = imu_samples.begin(); iter < imu_samples.end(); ++iter) {

				const ControlData::imu_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "imu_data: " << data << std::endl;

				}

			}
		}

	}

}

