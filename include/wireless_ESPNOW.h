#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <EEPROM.h>

typedef struct hc_message {
  uint8_t begin_validator[3];
  uint8_t sender_MAC_addr[6];
  uint8_t reciever_MAC_addr[6];
  uint8_t _sender;   //0 unknown, 1 controller, 2 termostat, 3 vent
  uint8_t _command;  //0 unknown, 1 register, 2 execute, 3 readState
  uint8_t mode;
  uint8_t batStat;
  uint8_t fanStatus;
  uint8_t setPoint_temp;
  uint8_t ventStatus;
  uint8_t reserved;
  uint8_t __hcdata; // 0 = do nothing
                    // 1 = cont.: introduce controller to host (reciver macAdd = controller MAC Add)
                    // 2 = host.: introduce host to controller (reciver macAdd = host IP)
                    // 3 = cont.: this is a message from a device (sender mac address)
                           // host : send this message directly to device (reciver mac addr) 
                    // 4 = host: pair a device with sender MAC Add
                    // 5 = host: unpair this device with reciever MAC Add if any
                    // 6 = cont.: delivery to device failed
                    // 7 = 
  uint8_t end_validator;
} hc_message;


void ESPNOW_wireless_init(void);
bool pairNew_device(uint8_t pair_addr[]);
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void ESPNOW_sendDataTo(uint8_t * mac_addr, uint8_t command, uint8_t *theData);