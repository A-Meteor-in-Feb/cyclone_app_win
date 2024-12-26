#include <algorithm>
#include <iostream>
#include <thread>
#include <future>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"


void run_subscriber_application(int vehicle_id) {

	int control_domain = 0;

	dds::domain::DomainParticipant control_participant(control_domain);

	dds::sub::Subscriber vehicle_subscriber(control_participant);

	dds::topic::Topic<ControlData::steeringWheel_data> steeringWheel_topic(control_participant, "steeringWheel_topic");
	dds::topic::Topic<ControlData::joyStick_data> joyStick_topic(control_participant, "joyStick_topic");

	dds::sub::DataReader<ControlData::steeringWheel_data> steeringWheel_reader(vehicle_subscriber, steeringWheel_topic);
	dds::sub::DataReader<ControlData::joyStick_data> joyStick_reader(vehicle_subscriber, joyStick_topic);

	dds::sub::LoanedSamples<ControlData::steeringWheel_data> sw_samples;
	dds::sub::LoanedSamples<ControlData::joyStick_data> js_samples;

	while (!shutdown_requested) {

		sw_samples = steeringWheel_reader.take();

		if (sw_samples.length() > 0) {

			dds::sub::LoanedSamples<ControlData::steeringWheel_data>::const_iterator iter;
			for (iter = sw_samples.begin(); iter < sw_samples.end(); ++iter) {

				const ControlData::steeringWheel_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "steeringWheel_data: " << data << std::endl;
				}
			}
		}

		js_samples = joyStick_reader.take();

		if (js_samples.length() > 0) {

			dds::sub::LoanedSamples<ControlData::joyStick_data>::const_iterator iter;
			for (iter = js_samples.begin(); iter < js_samples.end(); ++iter) {

				const ControlData::joyStick_data& data = iter->data();
				const dds::sub::SampleInfo& info = iter->info();

				if (info.valid()) {
					std::cout << "joyStic_data: " << data <<std::endl;
				}
			}
		}
	}

}