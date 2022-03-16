#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <SPI.h>

#include "host_handle.h"
#include "wireless_ESPNOW.h"

extern bool newData_flag_ESPNOW;
extern uint8_t myMAC_Address[], Brodcast_Address[], Controller_Address[], TERMO_Address[];
extern hc_message hc_mesg, ESPNOW_mesg;
uint8_t hc_sendFlag = 0, hc_recvFlag = 0;

bool host_available_flag = false;
TaskHandle_t CoreZEROTasks;

////////////////////////////////////////////////////////////////////////////////////
void coreZEROTasks_code( void * parameter) {
  for(;;) {
    //if (hc_sendFlag) send_data_to_host();
    if (receive_data_from_host())  hc_recvFlag = 1;
    if (hc_recvFlag) handle_host_message();
    delay(4);
  }
}
////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200); 
  Serial2.begin(115200); //, SERIAL_8N1, RXD2, TXD2);
            /* pin task to core 0 */ 

  ESPNOW_wireless_init();

   xTaskCreatePinnedToCore(
                    coreZEROTasks_code,      /* Task function. */
                    "Task1",        /* name of task. */
                    10000,          /* Stack size of task */
                    NULL,           /* parameter of the task */
                    1,              /* priority of the task */
                    &CoreZEROTasks, /* Task handle to keep track of created task */
                    0);   

  Serial.printf("My Controller MAC address is: %02X:%02X:%02X:%02X:%02X:%02X\n",
          myMAC_Address[0], myMAC_Address[1], myMAC_Address[2], myMAC_Address[3], myMAC_Address[4], myMAC_Address[5]);
  memcpy(ESPNOW_mesg.reciever_MAC_addr, myMAC_Address, 6);
  memcpy(ESPNOW_mesg.sender_MAC_addr, myMAC_Address, 6);
  memcpy(hc_mesg.reciever_MAC_addr, myMAC_Address, 6);
  memcpy(hc_mesg.sender_MAC_addr, myMAC_Address, 6);
  hc_mesg.begin_validator[0] = 'V';
  hc_mesg.begin_validator[1] = 'A';
  hc_mesg.begin_validator[2] = 'C';
  hc_mesg.end_validator = 'H';
  hc_mesg.__hcdata = 1; //introduce controller to host
  while(host_available_flag == false){
      Serial.println("Searching for Host...");
      Serial2.write((const uint8_t *)&hc_mesg, sizeof(hc_mesg));
      delay(1100);
  } 
}
////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  if (newData_flag_ESPNOW)
  {
    newData_flag_ESPNOW = false;
    delay(15); 
    //hc_sendFlag = 1;
    ESPNOW_mesg.__hcdata = 3; // 3 = cont.: this is a message from a device (sender mac address)
    send_data_to_host();
  }
  delay(1);
}
