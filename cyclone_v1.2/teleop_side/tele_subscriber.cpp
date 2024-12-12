#include <algorithm>
#include <iostream>
#include <thread>
#include <future>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "TeleData.hpp"

using namespace org::eclipse::cyclonedds;


int run_subscriber_application(int tele_id) {

	int domain_id = 0;

	dds::domain::DomainParticipant participant(domain_id);

	dds::sub::Subscriber tele_subscriber(participant);

	dds::topic::Topic<TeleData::streamdeck_buttons_data> buttons_topic(participant, "streamdeck_buttons_data");
	dds::topic::Topic<TeleData::imu_data> imu_topic(participant, "imu_data");

	dds::sub::DataReader<TeleData::streamdeck_buttons_data> buttons_reader(tele_subscriber, buttons_topic);
	dds::sub::DataReader<TeleData::imu_data> imu_reader(tele_subscriber, imu_topic);

	dds::sub::LoanedSamples<TeleData::streamdeck_buttons_data> buttons_samples;
	dds::sub::LoanedSamples<TeleData::imu_data> imu_samples;

	while (!shutdown_requested) {

		// RECEIVE AND TAKE THE DATA SAMPLE
		buttons_samples = buttons_reader.take();

		if (buttons_samples.length() > 0) {

			dds::sub::LoanedSamples<TeleData::streamdeck_buttons_data>::const_iterator iter;
			for (iter = buttons_samples.begin(); iter < buttons_samples.end(); ++iter) {

				const TeleData::streamdeck_buttons_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "streamdeck_buttons_data: " << data << std::endl;
					
				}

			}
		}

		imu_samples = imu_reader.take();

		if (imu_samples.length() > 0) {
			dds::sub::LoanedSamples<TeleData::imu_data>::const_iterator iter;
			for (iter = imu_samples.begin(); iter < imu_samples.end(); ++iter) {

				const TeleData::imu_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "imu_data: " << data << std::endl;

				}

			}
		}

	}

	return EXIT_SUCCESS;

}