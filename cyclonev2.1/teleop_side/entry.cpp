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


int run_publisher_application(int tele_id);
int run_subscriber_application(int tele_id);
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

            initControllers();
            std::thread tele_publisher(run_subscriber_application, tele_id);
            std::thread tele_subscriber(run_publisher_application, tele_id);

            tele_publisher.join();
            tele_subscriber.join();
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