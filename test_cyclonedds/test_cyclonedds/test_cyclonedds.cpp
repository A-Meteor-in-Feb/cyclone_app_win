#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "HelloWorldData.hpp"

using namespace org::eclipse::cyclonedds;

int main() {
    try {
        
        dds::domain::DomainParticipant participant(0);
        dds::topic::Topic<HelloWorldData::Msg> topic(participant, "HelloWorldData_Msg");
        dds::pub::Publisher publisher(participant);
        dds::pub::DataWriter<HelloWorldData::Msg> writer(publisher, topic);

        while (!shutdown_requested) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            HelloWorldData::Msg msg("Hello World");
            std::cout << "=== [Publisher] Write sample: " << msg << std::endl;
            writer.write(msg);
        }

    }
    catch (const dds::core::Exception& e) {
        std::cerr << "=== [Publisher] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "=== [Publisher] Done." << std::endl;

    return EXIT_SUCCESS;
}
