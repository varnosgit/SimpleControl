#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <EEPROM.h>

typedef struct hc_message { //16 character
  uint8_t begin_validator[3];
  uint8_t command;
  uint8_t reserved;
  uint8_t data[4];
  uint8_t MAC_addr[6];
  uint8_t end_validator;
} hc_message;

void send_data_to_host(void);
bool receive_data_from_host(void);
void handle_host_message(void);