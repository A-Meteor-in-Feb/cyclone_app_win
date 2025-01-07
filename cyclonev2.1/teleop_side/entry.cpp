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


void publisher_command_domain(int& tele_id, std::atomic<bool>& command_ato);
void subscriber_command_domain(int& tele_id, std::atomic<bool>& command_ato, std::atomic<bool>& control_ato);
void publisher_control_domain(int& tele_id, std::atomic<bool>& control_ato);
void subscriber_control_domain(int& tele_id, std::atomic<bool>& control_ato);


int main(int argc, char* argv[]) {

    int tele_id = -1;

    if (argc > 2 && strcmp(argv[1], "-id") == 0) {
        tele_id = atoi(argv[2]);
    }


    try{

        if (!shutdown_requested) {

            std::atomic<bool> command_ato = false;
            std::atomic<bool> control_ato = false;

            std::thread tele_publisher_command_domain(publisher_command_domain, std::ref(tele_id), std::ref(command_ato));
            std::thread tele_subscriber_command_domain(subscriber_command_domain, std::ref(tele_id), std::ref(command_ato), std::ref(control_ato));
            
            std::thread tele_subscriber_control_domain(publisher_control_domain, std::ref(tele_id), std::ref(control_ato));
            std::thread tele_publisher_control_domain(subscriber_control_domain, std::ref(tele_id), std::ref(control_ato));

            tele_publisher_control_domain.join();
            tele_subscriber_control_domain.join();

            tele_subscriber_command_domain.join();
            tele_publisher_command_domain.join();
            
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