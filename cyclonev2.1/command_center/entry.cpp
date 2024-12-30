#include <thread>
#include <algorithm>
#include <iostream>
#include "shutdownsignal.hpp"

//void run_publisher_application(std::atomic<bool>& p_conn);
void run_subscriber_application(std::atomic<bool>& p_conn);

int main(int argc, char* argv[]) {
	
	try {
		std::atomic<bool> p_conn;
		std::thread center_subscriber(run_subscriber_application, std::ref(p_conn));
		//std::thread center_publisher(run_publisher_application, std::ref(p_conn));

		center_subscriber.join();
		//center_publisher.join();
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