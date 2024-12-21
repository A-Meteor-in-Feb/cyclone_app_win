#include <algorithm>
#include <iostream>
#include <thread>
#include <future>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"

using namespace org::eclipse::cyclonedds;


int run_subscriber_application(int tele_id) {

	int domain_id = 0;

	dds::domain::DomainParticipant participant(domain_id);

	dds::sub::Subscriber tele_subscriber(participant);

	dds::topic::Topic<ControlData::imu_data> imu_topic(participant, "imu_data");

	dds::sub::DataReader<ControlData::imu_data> imu_reader(tele_subscriber, imu_topic);

	dds::sub::LoanedSamples<ControlData::imu_data> imu_samples;

	while (!shutdown_requested) {
		std::cout << "begin" << std::endl;

		imu_samples = imu_reader.take();

		if (imu_samples.length() > 0) {
			std::cout << "haha" << std::endl;
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

	return EXIT_SUCCESS;

}

int main(int argc, char* argv[]) {

    int tele_id = -1;

    if (argc > 2 && strcmp(argv[1], "-id") == 0) {
        tele_id = atoi(argv[2]);
    }


    try {
		run_subscriber_application(tele_id);
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