#ifndef SHUTDOWNSIGNAL_HPP
#define SHUTDOWNSIGNAL_HPP

#include <iostream>
#include <csignal>


extern bool shutdown_requested;

inline void stop_handler(int sig)
{
    shutdown_requested = true;
    std::cout << "preparing to shut down..." << std::endl;
}

inline void setup_signal_handlers()
{
    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);
}

#endif