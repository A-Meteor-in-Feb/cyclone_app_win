#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

#include <string>
#include <locale>
#include <codecvt>
#include <bitset>

#include "shutdownsignal.hpp"


int run_publisher_application(int tele_id);
int run_subscriber_application(int tele_id);
void initControllers();


int main(int argc, char* argv[]) {

    int tele_id = -1;

    if (argc > 2 && strcmp(argv[1], "-id") == 0) {
        tele_id = atoi(argv[2]);
    }


    try {

        initControllers();
        std::thread tele_publisher(run_subscriber_application, tele_id);
        std::thread tele_subscriber(run_publisher_application, tele_id);

        tele_publisher.join();
        tele_subscriber.join();

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