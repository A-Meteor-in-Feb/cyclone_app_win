#include <string>
#include <locale>
#include <codecvt>
#include <bitset>

#include "dds/dds.hpp"
#include "shutdownsignal.hpp"
#include "ControlData.hpp"

#include "LogitechSteeringWheelLib.h"

#pragma comment(lib, "LogitechSteeringWheelLib.lib")

#define MAX_NUMBER_OF_CONTROLLERS 5

using namespace org::eclipse::cyclonedds;

wchar_t name[256];
int wheelIndex = -1;
//int joystickIndex = -1;

DIJOYSTATE2* steeringWheel_state;
//DIJOYSTATE2* joyStick_state;



LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {

    case WM_DESTROY: 
        PostQuitMessage(0);
        return 0;
    
    default:
        return ::DefWindowProc(hwnd, message, wparam, lparam);
    }

}


std::string wstringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::bitset<32> getBitwiseButtons(DIJOYSTATE2* state, int max_buttons) {
    std::bitset<32> buttons;
    for (int i = 0; i < max_buttons; ++i) {
        if (state && (state->rgbButtons[i] & 0x80)) {
            buttons.set(i);
        }
    }

    return buttons;
}


std::bitset<32> getCombinedButtons(DIJOYSTATE2* state) {
    std::bitset<32> buttons = getBitwiseButtons(state, 24);
    if (state) {
        switch (state->rgdwPOV[0]) {
        case 0: buttons.set(24); break;      //up
        case 4500: buttons.set(25); break;   //up-right
        case 9000: buttons.set(26); break;   //right
        case 13500: buttons.set(27); break;  //down-right
        case 18000: buttons.set(28); break;  //down
        case 22500: buttons.set(29); break;  //down-left
        case 27000: buttons.set(30); break;  //left
        case 31500: buttons.set(31); break;  //up-left
        default: break;                      //no direction
        }
    }
    return buttons;

}



void initControllers() {

    while (!LogiSteeringInitialize(true) && !shutdown_requested) {
        std::cerr << "Error - couldn't detect any controller. Are you running as administrator?" << std::endl;
        LogiUpdate();
        Sleep(100);
    }

    if (!shutdown_requested) {

        for (int i = 0; i < MAX_NUMBER_OF_CONTROLLERS; i++) {

            if (LogiIsConnected(i)) {
                LogiGetFriendlyProductName(i, name, sizeof(name));
                std::wstring wname(name);
                std::string nameStr = wstringToString(wname);

                if (nameStr.find("wheel") != std::string::npos || nameStr.find("Wheel") != std::string::npos) {
                    wheelIndex = i;
                    std::cout << "Wheel detected at index: " << wheelIndex << std::endl;
                }/*
                else if (nameStr.find("stick") != std::string::npos || nameStr.find("Stick") != std::string::npos) {
                    joystickIndex = i;
                    std::cout << "JoyStick detected at index: " << joystickIndex << std::endl;
                }*/
            }
        }
    }
}



int run_publisher_application(int tele_id) {

    std::string tele_name = "tele" + std::to_string(tele_id);

    LogiPlayLeds(wheelIndex, 4, 1, 6);
    LogiSetOperatingRange(wheelIndex, 900);

    int domain_id = 0;

    dds::domain::DomainParticipant participant(domain_id);

    dds::pub::Publisher tele_publisher(participant);

    dds::topic::Topic<ControlData::steeringWheel> steeringWheel_topic(participant, "steeringWheel_topic");

    dds::pub::DataWriter<ControlData::steeringWheel> steeringWheel_writer(tele_publisher, steeringWheel_topic);


    while (!shutdown_requested) {

        if (GetKeyState('Q') < 0) {
            break;
        }

        //then, transmit the data...
        if (LogiUpdate()) {

            std::cout << "update" << std::endl;

            try {
                steeringWheel_state = LogiGetState(wheelIndex);

                if (steeringWheel_state) {

                    std::bitset<32> wheelButtons = getCombinedButtons(steeringWheel_state);

                    //Data needed to transmit
                    long sw_lX = steeringWheel_state->lX;
                    long sw_lY = steeringWheel_state->lY;
                    long sw_lRz = steeringWheel_state->lRz;
                    long sw_rglSlider_0 = steeringWheel_state->rglSlider[0];
                    unsigned long sw_buttons = wheelButtons.to_ulong();

                    ControlData::steeringWheel steeringWheel_data(sw_lX, sw_lY, sw_lRz, sw_rglSlider_0, sw_buttons);

                    steeringWheel_writer.write(steeringWheel_data);

                    std::cout << "wrote." << std::endl;

                }
                else {
                    std::cout << "controller is nullptr" << std::endl;
                }


            }
            catch (const std::exception& e) {
                std::cerr << "Standard Exception: " << e.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Unknown Error" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(33));
        }
    }

    return EXIT_SUCCESS;
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
            40,20,50,50,
            NULL,
            NULL,
            hInstance,
            NULL
        );
        if (hwnd == NULL) {
            std::cerr << "Failed to create a Window" << std::endl;
            return 0;
        }
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        initControllers();

        if (!shutdown_requested) {
            MSG msg = {};
            if (GetMessage(&msg, NULL, 0, 0)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            run_publisher_application(tele_id);
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception in publication function." << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown Error :(" << std::endl;
    }

    return EXIT_SUCCESS;
}