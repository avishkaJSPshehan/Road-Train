/*
##########################             Remote             ##########################
*/

//----------------------------------------Load libraries
#include <esp_now.h>
#include <WiFi.h>
//----------------------------------------

uint8_t broadcastAddress[] = { 0x08, 0xD1, 0xF9, 0xE8, 0x69, 0xD0 }; // Replace with the MAC Address of your receiver.

int send_rnd_val_1;
int send_rnd_val_2;
int send_rnd_val_3;
int send_rnd_val_4;

int angle = 34;
int speed = 35;

String success; // Variable to store if sending data was successful.

typedef struct struct_message {
    int rnd_1;
    int rnd_2;
    int rnd_3;
    int rnd_4;
} struct_message;

struct_message send_Data; // Create a struct_message to send data.

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    success = (status == ESP_NOW_SEND_SUCCESS) ? "Delivery Success :)" : "Delivery Fail :(";
    Serial.println(">>>>>");
}

void setup() {
    Serial.begin(115200);
    pinMode(speed, INPUT);
    pinMode(angle, INPUT);
    pinMode(25, INPUT);
    pinMode(26, OUTPUT);
    digitalWrite(26, HIGH);
    
    WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station.

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    send_rnd_val_1 = analogRead(angle);
    send_rnd_val_2 = analogRead(speed);
    int val = digitalRead(25);

    int mappingValue = map(send_rnd_val_1, 0, 4095, -45, 45);
    int motor_speed = map(send_rnd_val_2, 0, 4095, 0, 255);
    int servoValue = map(mappingValue, -45, 45, 18, 108);

    send_Data.rnd_1 = servoValue;
    send_Data.rnd_2 = motor_speed;
    send_Data.rnd_3 = mappingValue;
    send_Data.rnd_4 = val;

    Serial.println();
    Serial.print("Servo Angle >>> ");
    Serial.print(servoValue);
    Serial.print("      Motor Speed >>> ");
    Serial.print(motor_speed);

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
   
    if (result == ESP_OK) {
        Serial.println("Sent with success");
    } else {
        Serial.println("Error sending the data");
    }
}
