/*
##########################             Server             ##########################
*/

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

int ENA = 13;
int ENB = 14;
int IN1 = 5;
int IN2 = 4;
int IN3 = 33;
int IN4 = 32;

const int frequency = 500;
const int pwm_channel = 2;
const int pwm_channel_2 = 3;
const int resolution = 8;

int receive_rnd_val_1;
int receive_rnd_val_2;
int receive_rnd_val_3;
int receive_rnd_val_4;

Servo Steering;
Servo Steering_back;

typedef struct struct_message {
    int rnd_1;
    int rnd_2;
    int rnd_3;
    int rnd_4;
} struct_message;

struct_message receive_Data; 

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&receive_Data, incomingData, sizeof(receive_Data));

    receive_rnd_val_1 = receive_Data.rnd_1;
    receive_rnd_val_2 = receive_Data.rnd_2;
    receive_rnd_val_3 = receive_Data.rnd_3;
    receive_rnd_val_4 = receive_Data.rnd_4;

    Serial.print("Steering Angle  : >>>  ");
    Serial.print(receive_rnd_val_3);
    Serial.print("     Servo Angle  : >>>  ");
    Serial.print(receive_rnd_val_1);
    Serial.print("     Motor Speed >>>  ");
    Serial.print(receive_rnd_val_2);
    Serial.print("     Motor Side >>>  ");
    Serial.print(receive_rnd_val_4);

    Steering.write(receive_rnd_val_1);
    int back_steering_angle = back_steering_angle_cal(receive_rnd_val_1);
    Steering_back.write(back_steering_angle);

    Serial.print("     Back Steering Angle >>>  ");
    Serial.println(back_steering_angle);

    changeSpeed(receive_rnd_val_2, receive_rnd_val_4);
}

int back_steering_angle_cal(int value) {
    return map(value, 18, 108, 85, 41);
}

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    Steering.attach(19);
    Steering_back.attach(18);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    ledcSetup(pwm_channel, frequency, resolution);
    ledcSetup(pwm_channel_2, frequency, resolution);
    ledcAttachPin(ENA, pwm_channel);
    ledcAttachPin(ENB, pwm_channel_2);

    Serial.begin(115200);
    pinMode(2, OUTPUT);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
}

void changeSpeed(int mappingValue, int side) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    ledcWrite(pwm_channel, mappingValue);
    ledcWrite(pwm_channel_2, mappingValue);
}
