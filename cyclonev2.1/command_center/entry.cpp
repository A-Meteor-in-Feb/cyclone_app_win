#include <thread>
#include <algorithm>
#include <iostream>
#include "shutdownsignal.hpp"

void run_publisher_application(std::atomic<bool>& p_conn);
void run_subscriber_application(std::atomic<bool>& p_conn);

int main(int argc, char* argv[]) {
	
	try {
		std::atomic<bool> p_conn;
		std::thread center_subscriber(run_subscriber_application, std::ref(p_conn));
		std::thread center_publisher(run_publisher_application, std::ref(p_conn));

		center_subscriber.join();
		center_publisher.join();
	}

	return EXIT_SUCCESS;
}