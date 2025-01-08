#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

#include <string>
#include <locale>
#include <codecvt>
#include <bitset>

#include <Windows.h>
#include "shutdownsignal.hpp"


void run_command_domain(int& tele_id);
void publisher_control_domain(int& tele_id);
void subscriber_control_domain(int& tele_id);


int main(int argc, char* argv[]) {

    int tele_id = -1;

    if (argc > 2 && strcmp(argv[1], "-id") == 0) {
        tele_id = atoi(argv[2]);
    }


    try{

        if (!shutdown_requested) {

            std::thread tele_command_domain(run_command_domain, std::ref(tele_id));
            std::thread tele_control_publisher(publisher_control_domain, std::ref(tele_id));
            std::thread tele_control_subscriber(subscriber_control_domain, std::ref(tele_id));

            tele_command_domain.join();
            tele_control_publisher.join();
            tele_control_subscriber.join();
            
        }

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