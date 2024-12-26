#include <algorithm>
#include <iostream>
#include <string>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"



void run_subscriber_application(int vehicle_id) {

    int domain_id = 0;

    dds::domain::DomainParticipant participant(domain_id);

    dds::sub::Subscriber vehicle_subscriber(participant);

    dds::topic::Topic<ControlData::steeringWheel> steeringWheel_topic(participant, "steeringWheel_topic");

    dds::sub::DataReader<ControlData::steeringWheel> steeringWheel_reader(vehicle_subscriber, steeringWheel_topic);

    dds::sub::LoanedSamples<ControlData::steeringWheel> sw_samples;

    while (!shutdown_requested) {
        sw_samples = steeringWheel_reader.take();

        if (sw_samples.length() > 0) {

            dds::sub::LoanedSamples<ControlData::steeringWheel>::const_iterator iter;

            for (iter = sw_samples.begin(); iter < sw_samples.end(); ++iter) {

                const ControlData::steeringWheel& data = iter->data();
                const dds::sub::SampleInfo& info = iter->info();

                if (info.valid()) {
                    std::cout << "steering wheel data: " << data << std::endl;
                }
            }
        }
    }

}


int main(int argc, char* argv[]) {

    int vehicle_id = -1;

    if (argc > 2 && strcmp(argv[1], "-id") == 0) {
        vehicle_id = atoi(argv[2]);
    }

    run_subscriber_application(vehicle_id);

    return EXIT_SUCCESS;
}