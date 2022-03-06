#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <EEPROM.h>


void send_data_to_host(void);
bool receive_data_from_host(void);
void handle_host_message(void);