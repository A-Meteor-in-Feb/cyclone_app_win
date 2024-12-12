#include <algorithm>
#include <iostream>
#include <thread>
#include <future>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "VehicleData.hpp"


void run_subscriber_application(int vehicle_id) {

	int domain_id = 0;

	dds::domain::DomainParticipant participant(domain_id);

	dds::sub::Subscriber vehicle_subscriber(participant);

	dds::topic::Topic<VehicleData::steeringWheel_data> steeringWheel_topic(participant, "steeringWheel_topic");
	dds::topic::Topic<VehicleData::joyStick_data> joyStick_topic(participant, "joyStick_topic");

	dds::sub::DataReader<VehicleData::steeringWheel_data> steeringWheel_reader(vehicle_subscriber, steeringWheel_topic);
	dds::sub::DataReader<VehicleData::joyStick_data> joyStick_reader(vehicle_subscriber, joyStick_topic);

	dds::sub::LoanedSamples<VehicleData::steeringWheel_data> sw_samples;
	dds::sub::LoanedSamples<VehicleData::joyStick_data> js_samples;

	while (!shutdown_requested) {

		sw_samples = steeringWheel_reader.take();

		if (sw_samples.length() > 0) {

			dds::sub::LoanedSamples<VehicleData::steeringWheel_data>::const_iterator iter;
			for (iter = sw_samples.begin(); iter < sw_samples.end(); ++iter) {

				const VehicleData::steeringWheel_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "steeringWheel_data: " << data << std::endl;
				}
			}
		}

		js_samples = joyStick_reader.take();

		if (js_samples.length() > 0) {

			dds::sub::LoanedSamples<VehicleData::joyStick_data>::const_iterator iter;
			for (iter = js_samples.begin(); iter < js_samples.end(); ++iter) {

				const VehicleData::joyStick_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "joyStic_data: " << data <<std::endl;
				}
			}
		}
	}

}