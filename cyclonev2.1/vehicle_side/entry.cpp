#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

#include <string>

#include "shutdownsignal.hpp"


void run_publisher_application(int vehicle_id);
void run_subscriber_application(int vehicle_id);


int main(int argc, char* argv[]) {

    int vehicle_id = -1;

    if (argc > 2 && strcmp(argv[1], "-id") == 0) {
        vehicle_id = atoi(argv[2]);
    }

    std::thread vehicle_publisher(run_publisher_application, vehicle_id);
    std::thread vehicle_subscriber(run_subscriber_application, vehicle_id);

    vehicle_publisher.join();
    vehicle_subscriber.join();

    return EXIT_SUCCESS;

}