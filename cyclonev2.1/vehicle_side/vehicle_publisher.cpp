#include "dds/dds.hpp"
#include "ControlData.hpp"
#include "shutdownsignal.hpp"

#include "string.h"
#include "Math.h"
#include "windows.h"
#include "Com.h"

#include<stdio.h>
#include<conio.h>
#include "wit_c_sdk.h"


static char s_cDataUpdate = 0;
int iComPort = 4;
int iBaud = 9600;
int iAddress = 0x50;
void ComRxCallBack(char* p_data, UINT32 uiSize)
{
    for (UINT32 i = 0; i < uiSize; i++)
    {
        WitSerialDataIn(p_data[i]);
    }
}

static void AutoScanSensor(void);
static void SensorUartSend(uint8_t* p_data, uint32_t uiSize);
static void CopeSensorData(uint32_t uiReg, uint32_t uiRegNum);
static void DelayMs(uint16_t ms);




void run_publisher_application(int vehicle_id) {

    std::string vehicle_name = "vehilce" + std::to_string(vehicle_id);

    int control_domain = 1;

    dds::domain::DomainParticipant control_participant(control_domain);

    dds::topic::Topic<ControlData::imu_data> imu_topic(control_participant, "imu_data");
    dds::topic::Topic<ControlData::statistic_data> statistic_topic(control_participant, "statistic_data");

    dds::pub::Publisher vehicle_publisher(control_participant);

    dds::pub::DataWriter<ControlData::imu_data> imu_writer(vehicle_publisher, imu_topic);
    dds::pub::DataWriter<ControlData::statistic_data> statistic_writer(vehicle_publisher, statistic_topic);

    
    printf("Please enter the serial number:");
    scanf_s("%d", &iComPort);

    OpenCOMDevice(iComPort, iBaud);
    WitInit(WIT_PROTOCOL_NORMAL, 0x50);
    WitSerialWriteRegister(SensorUartSend);
    WitRegisterCallBack(CopeSensorData);
    AutoScanSensor();

    float a[3], w[3], Angle[3], h[3];

    //Write and Publish the data topics;
    while (!shutdown_requested) {
        
        for (int i = 0; i < 3; i++) {

            a[i] = (float)sReg[AX + i] / 32768.0f * 16.0f;
            w[i] = (float)sReg[GX + i] / 32768.0f * 2000.0f;
            Angle[i] = (float)sReg[Roll + i] / 32768.0f * 180.0f;
            h[i] = (float)sReg[HX + i];
        }

        ControlData::imu_data imu_data(vehicle_name, { a[0], a[1], a[2] }, { w[0], w[1], w[2] }, { Angle[0], Angle[1], Angle[2] }, { h[0], h[1], h[2] });
        imu_writer.write(imu_data);

        int sleep = 0;
        while (sleep < 5) {
            ControlData::statistic_data statistic_data(1.1, 2.2, 0);
            statistic_writer.write(statistic_data);

            std::this_thread::sleep_for(std::chrono::microseconds(10));
            sleep += 1;
        }

        
    }
}

static void DelayMs(uint16_t ms)
{
    Sleep(ms);
}

static void SensorUartSend(uint8_t* p_data, uint32_t uiSize)
{
    SendUARTMessageLength((const char*)p_data, uiSize);
}
static void CopeSensorData(uint32_t uiReg, uint32_t uiRegNum)
{
    s_cDataUpdate = 1;
}

static void AutoScanSensor(void)
{
    const uint32_t c_uiBaud[7] = { 4800, 9600, 19200, 38400, 57600, 115200, 230400 };
    int i, iRetry;

    for (i = 0; i < 7; i++)
    {
        CloseCOMDevice();
        OpenCOMDevice(iComPort, c_uiBaud[i]);
        //SetBaundrate(c_uiBaud[i]);
        iRetry = 2;
        do
        {
            s_cDataUpdate = 0;
            WitReadReg(AX, 3);
            Sleep(100);
            if (s_cDataUpdate != 0)
            {
                printf("%d baud find sensor\r\n\r\n", c_uiBaud[i]);
                return;
            }
            iRetry--;
        } while (iRetry);
    }
    printf("can not find sensor\r\n");
    printf("please check your connection\r\n");
}