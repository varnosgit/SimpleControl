#include "host_handle.h"
#include "wireless_ESPNOW.h"

extern uint8_t myMAC_Address[], Brodcast_Address[];
extern hc_message ESPNOW_mesg;
hc_message hc_mesg;
uint8_t hc_sendFlag = 0, hc_recvFlag = 0;

void send_data_to_host(void)
{   
    Serial2.write((const uint8_t *)&ESPNOW_mesg, sizeof(ESPNOW_mesg));  
    Serial.printf("Reciever Address is: %02X:%02X:%02X:%02X:%02X:%02X\n",
          ESPNOW_mesg.reciever_MAC_addr[0], ESPNOW_mesg.reciever_MAC_addr[1], 
          ESPNOW_mesg.reciever_MAC_addr[2], ESPNOW_mesg.reciever_MAC_addr[3], 
          ESPNOW_mesg.reciever_MAC_addr[4], ESPNOW_mesg.reciever_MAC_addr[5]);
    hc_sendFlag = 0;
}

bool receive_data_from_host(void)
{
    if (Serial2.available()) 
    {
      char* rxdata = (char*) &hc_mesg;
      Serial2.read(rxdata, sizeof(hc_mesg));

      Serial.println((char *)rxdata);
      return true;
    }
    return false;
}

void handle_host_message(void)
{   
  if (hc_mesg.__hcdata == 4) // 4 = host: pair a device with reciever MAC Add
  {
    pairNew_device(hc_mesg.reciever_MAC_addr);
  }
  if (hc_mesg.__hcdata == 5) // 5 = host: unpair this device with reciever MAC Add if any
  {
    esp_now_del_peer(hc_mesg.reciever_MAC_addr);
  }

  if (hc_mesg.__hcdata == 6) // 6 = host: directly send this message to a device with reciever MAC Add
  {
    memcpy(hc_mesg.sender_MAC_addr, myMAC_Address, 6);
    if (esp_now_send(hc_mesg.reciever_MAC_addr, (uint8_t *)&hc_mesg, sizeof(hc_mesg)) == ESP_OK)
    {  
      Serial.println("datasSent with success.");
    }
    hc_recvFlag = 0;
  }

   
}
