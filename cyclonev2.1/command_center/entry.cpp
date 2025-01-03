#include <thread>
#include <algorithm>
#include <iostream>
#include "shutdownsignal.hpp"

void run_subscriber_application();

int main(int argc, char* argv[]) {
	
	try {
		std::thread center_subscriber(run_subscriber_application);
		

		center_subscriber.join();
		
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