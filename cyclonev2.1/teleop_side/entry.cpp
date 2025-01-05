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


void publisher_command_domain(const int& tele_id, std::atomic<bool>& command_ato);
void subscriber_command_domain(const int& tele_id, std::atomic<bool>& command_ato, std::atomic<bool>& control_ato);
void publisher_control_domain(const int& tele_id, std::atomic<bool>& control_ato);
void subscriber_control_domain(const int& tele_id, std::atomic<bool>& control_ato);
void initControllers();


LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return ::DefWindowProc(hwnd, message, wparam, lparam);
    }

}


int main(int argc, char* argv[]) {

    int tele_id = -1;

    if (argc > 2 && strcmp(argv[1], "-id") == 0) {
        tele_id = atoi(argv[2]);
    }


    try {

        HINSTANCE hInstance = GetModuleHandle(NULL);
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = "Name";
        RegisterClass(&wc);
        HWND hwnd = CreateWindowEx(
            0,
            "Name",
            "Name",
            WS_OVERLAPPEDWINDOW,
            40, 20, 50, 50,
            NULL,
            NULL,
            hInstance,
            NULL
        );

        if (hwnd == NULL) {
            std::cerr << "Failed to create a Window" << std::endl;
            return 0;
        }

        if (!shutdown_requested) {
            MSG msg = {};
            if (GetMessage(&msg, NULL, 0, 0)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            std::atomic<bool> command_ato = false;
            std::atomic<bool> control_ato = false;

            initControllers();

            std::thread tele_subscriber_command_domain(subscriber_command_domain, tele_id, std::ref(command_ato), std::ref(control_ato));
            std::thread tele_publisher_command_domain(publisher_command_domain, tele_id, std::ref(command_ato));
            
            if (control_ato.load()) {

                std::thread tele_subscriber_control_domain(publisher_control_domain, tele_id, std::ref(control_ato));
                std::thread tele_publisher_control_domain(subscriber_control_domain, tele_id, std::ref(control_ato));

                tele_publisher_control_domain.join();
                tele_subscriber_control_domain.join();

            }

            tele_subscriber_command_domain.join();
            tele_publisher_command_domain.join();
            
        }
        else {
            PostMessage(HWND_BROADCAST, WM_DESTROY, 0, 0);
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