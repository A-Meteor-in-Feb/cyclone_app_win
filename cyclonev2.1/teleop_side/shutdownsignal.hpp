#ifndef SHUTDOWNSIGNAL_HPP
#define SHUTDOWNSIGNAL_HPP

#include <iostream>
#include <csignal>
#include <atomic>


extern std::atomic<bool> shutdown_requested;

inline void stop_handler(int sig)
{
    shutdown_requested.store(true, std::memory_order_release);
    std::cout << "preparing to shut down..." << std::endl;
}

inline void setup_signal_handlers()
{
    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);
}

#endif