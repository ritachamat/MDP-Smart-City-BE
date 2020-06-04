//Motion Light Temperature detection using PIR LDR LM35 sensor

#include "ArduinoJson.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


const char* ssid = "TP-LINK_42523E";
const char* password =  "42042280";
const char* mqttServer = "18.191.57.118";
const int mqttPort = 8883;
const char* mqttUser = "abribus1";
const char* mqttPassword = "pass";

const char* test_root_ca = \
                           "-----BEGIN CERTIFICATE-----\n" \
                           "MIIECzCCAvOgAwIBAgIUQsX8s7W9GugDhlKuNbKc2D/6rfEwDQYJKoZIhvcNAQEL\n" \
                           "BQAwgZQxCzAJBgNVBAYTAkxCMRAwDgYDVQQIDAdMZWJhbm9uMQ8wDQYDVQQHDAZC\n" \
                           "ZWlydXQxDTALBgNVBAoMBHByb2oxDDAKBgNVBAsMA21kcDEdMBsGA1UEAwwUcHJv\n" \
                           "amV0MDQuZHVja2Rucy5vcmcxJjAkBgkqhkiG9w0BCQEWF3JpdGEuY2hhbWF0QGhv\n" \
                           "dG1haWwuY29tMB4XDTIwMDUxOTEzNTYzMFoXDTIzMDIxMzEzNTYzMFowgZQxCzAJ\n" \
                           "BgNVBAYTAkxCMRAwDgYDVQQIDAdMZWJhbm9uMQ8wDQYDVQQHDAZCZWlydXQxDTAL\n" \
                           "BgNVBAoMBHByb2oxDDAKBgNVBAsMA21kcDEdMBsGA1UEAwwUcHJvamV0MDQuZHVj\n" \
                           "a2Rucy5vcmcxJjAkBgkqhkiG9w0BCQEWF3JpdGEuY2hhbWF0QGhvdG1haWwuY29t\n" \
                           "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2VKViYM6a1lJxbaUvuGy\n" \
                           "1z4MVkze1d4XQuk+6EeZvwcnhbVxgjEh3EfULgXh4eJBUu/RphpPjXnuiw68UWVy\n" \
                           "7LpLqSo0QjsWErWtfwXnF99ljjlYoQw7rxYgMS0n+7Vj7+CaX8Anq/rQ3aRkaOhp\n" \
                           "23zbxoIAFgEcfMTrMzL6glt2bxpfs9BLagNDmLFNmgqDL5aeS4YQrYWOJdSc9oiI\n" \
                           "mWxDIRMoBIOCIJs5Su7DXynvrnCnzmBT5ddEkZsJ3DiqTOyHIiwjHvi5t1aMNW3z\n" \
                           "ooUX9Bmzm1uUwgy613ZbwgfbwhOCogB7wB64CfNjYj9sMTRKvf+abdLq5BGq/qjl\n" \
                           "NwIDAQABo1MwUTAdBgNVHQ4EFgQUTTfKz4Cf3dfPBeOpf/yrBm2JagkwHwYDVR0j\n" \
                           "BBgwFoAUTTfKz4Cf3dfPBeOpf/yrBm2JagkwDwYDVR0TAQH/BAUwAwEB/zANBgkq\n" \
                           "hkiG9w0BAQsFAAOCAQEANKxT8rb59ZIzcqLV5UUTyByHLvQhiIattGkmkk0hS/c9\n" \
                           "iLJkcyPEvupwA6rey8ohmrxaQL1X/yo7gbtcFoFkSglfvO3MVaLBTJQU1q6Z7vPp\n" \
                           "go5VqWhRKXxmmCFd5YKVXoxcNJhJ5ECuphnHuSxlSGsiHjer04F0I29lfMQuxTK0\n" \
                           "kaiCvY1FqHwl6zj3u52S3b0wvqrfr4ENWuFYvaZd8gdiKLdsavHRZc2teafIWKqn\n" \
                           "iY+PVlEQI4k2V+Q46Ze7huoNpKI5g3wBVptk6CDAh7Euw7/75VoXn2yk1oJ5NmJ2\n" \
                           "Wq3UZviv6TIdXLU5mJVS6hqvIR+ew0H+37gGCgdHQg==\n" \
                           "-----END CERTIFICATE-----\n";

const char* test_client_cert = \
                               "-----BEGIN CERTIFICATE-----\n" \
                               "MIIDoTCCAokCFDT56Y/RvISP9JOjQVM0hhtGLgefMA0GCSqGSIb3DQEBCwUAMIGU\n" \
                               "MQswCQYDVQQGEwJMQjEQMA4GA1UECAwHTGViYW5vbjEPMA0GA1UEBwwGQmVpcnV0\n" \
                               "MQ0wCwYDVQQKDARwcm9qMQwwCgYDVQQLDANtZHAxHTAbBgNVBAMMFHByb2pldDA0\n" \
                               "LmR1Y2tkbnMub3JnMSYwJAYJKoZIhvcNAQkBFhdyaXRhLmNoYW1hdEBob3RtYWls\n" \
                               "LmNvbTAeFw0yMDA1MTkxNDUxMjBaFw0yMzAyMTMxNDUxMjBaMIGEMQswCQYDVQQG\n" \
                               "EwJMQjEQMA4GA1UECAwHTGViYW5vbjEPMA0GA1UEBwwGQmVpcnV0MQ0wCwYDVQQK\n" \
                               "DARwcm9qMQwwCgYDVQQLDANtZHAxDTALBgNVBAMMBGJ1czExJjAkBgkqhkiG9w0B\n" \
                               "CQEWF3JpdGEuY2hhbWF0QGhvdG1haWwuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
                               "AQ8AMIIBCgKCAQEA7ghBpKjZivnyTni7IeBwu8kAanHRme86PHtN/CxqsgP8QtRe\n" \
                               "GWOUsG/MUvHGxquGWQnEMw2A/23ykbejuvR+tgVILwyecnbYQqj5hnfD4jQQGedX\n" \
                               "4RtuLLDC3KKAhm9mHIjQNTyDDJzkZB74X2ZrLK0sbWZanDmwtq9q446/blKXRo/3\n" \
                               "v41Bb2PabnAI2jnd2dSVq1I+6ZrRo+rdjzCTPCl9tzCPOX6x2kfnE5MTXMhWpU5X\n" \
                               "923T4aYCVhnGUdarIA/xnHjwwCoQhUeTKDxbfjJY0fPIC65FzUeG5crD4QYj/zpJ\n" \
                               "pM4OkUbwx6jduYGEl9u8byVLBzL6EMA5Fer01QIDAQABMA0GCSqGSIb3DQEBCwUA\n" \
                               "A4IBAQBEQOOZiJs7MSFnx0a7CIXkF1XKvNZEkc2yJkQci3XJbfr0DYB0mLeXxSJd\n" \
                               "JMpKvhcXeNEb2LOFJX5hif1ogMuqneV4jKOPCt3Fdd9PvtUAoFiy6l9n2aQTzZbb\n" \
                               "iyNcb1nzA2ub+aYXzcEeqqZRskF07BlLui6zMsUZUfMiRnHX7Y+O9+O1EH2WVODx\n" \
                               "FdAtMOu0geqmlyB0gMg0pTY4ZOMGEPJBFcTHI36JcrPERhxJLICbgvNeeG81KksW\n" \
                               "Zd5r+5a4catKcq1Y6ES7ov6Ftt+hLkwvXuIJOq4eQy51BOA39dWRZX3eUy4gsZBS\n" \
                               "OxycJBS3Het7XGwKZRn0oWFS38oU\n" \
                               "-----END CERTIFICATE-----\n";



const char* test_client_key = \
                              "-----BEGIN RSA PRIVATE KEY-----\n" \
                              "MIIEpAIBAAKCAQEA7ghBpKjZivnyTni7IeBwu8kAanHRme86PHtN/CxqsgP8QtRe\n" \
                              "GWOUsG/MUvHGxquGWQnEMw2A/23ykbejuvR+tgVILwyecnbYQqj5hnfD4jQQGedX\n" \
                              "4RtuLLDC3KKAhm9mHIjQNTyDDJzkZB74X2ZrLK0sbWZanDmwtq9q446/blKXRo/3\n" \
                              "v41Bb2PabnAI2jnd2dSVq1I+6ZrRo+rdjzCTPCl9tzCPOX6x2kfnE5MTXMhWpU5X\n" \
                              "923T4aYCVhnGUdarIA/xnHjwwCoQhUeTKDxbfjJY0fPIC65FzUeG5crD4QYj/zpJ\n" \
                              "pM4OkUbwx6jduYGEl9u8byVLBzL6EMA5Fer01QIDAQABAoIBAQDhoevvlKraBUH9\n" \
                              "xo3EeAcEymzdedSwsUcny85JqyTcZ9C43KvyLyXj9aXePhcpfbAVOJ/FsRhFiEg2\n" \
                              "frfyYQ63pdwd1T1gzW0ECz60i1SgWK+mjlgMJz9Rl6CvGk7woCpQBBQxUpH648HP\n" \
                              "clgHvoL7T5Bch3h+vGTZVwDgghDOFiH+SlFKKG9MTGKfi2Q3dtNn4xms5AN1UlLJ\n" \
                              "74phYGzoUCAciG9Tlknwt4PXQrXiCdMjTODLmo5vKfUaXdfTdoH37PYY+ZHYPJj7\n" \
                              "B6cCkL5xfbDDo3t4BX8nRv92yIts+dVPJiNkW+9jgZb4WwBY3F7Fk4ukeJrrZaKo\n" \
                              "Qq549eCFAoGBAPvuUhq9fk0sZKkESSrZsyeg0x1VnEZbhb5xUzik6k2dk820zvA5\n" \
                              "3jS4CXmakjaonIQxKZMHWriBXS1+OLLf6i4wA97/tZxVMD4bFHBnR4LcOmhi7xD8\n" \
                              "j6+0d5zJsTMV49WfHeROHH0Kh7s8UHHFpFTnIsGil6XPKenvdKHkJKSHAoGBAPHg\n" \
                              "d7jM2/ouK90KV4LCNd+dHrBREm/40UpiEUDl7YQI+QXuOmQfr0SlIO6u3WSwYZYP\n" \
                              "3/eRm04jUrMkPjb7ZfgYkE+X0s716bEnHzgwhXri54hzYrDWrzpJADdTyPPZN9Db\n" \
                              "uwz9UsCxHh5n6yNGCxX+LUkte6spQXcHPegFu87DAoGAdSYmQc76C1LoX/vhNi7T\n" \
                              "c0sCwLJNoB3w/FTgOQfe+EadM1uvmt+d52/o4AYzlX0kgwgFaBxUYFWviXcXUH50\n" \
                              "/BNRyYGo5tBqRYb8IySziTN1aZMPzhtTiYXm1R6o6rN0JaL/GGoGL1MVOMjcM3uP\n" \
                              "ZfexHs/Hqs/BksnJt3pKW8MCgYEAjRo9vXkE+zXkMNbPpmBVvVWTiZhCAUBOF+6N\n" \
                              "0iZQIS5615LaTX6OaKVMSPGKBis/QqOQLISs7TB+FjOWd8zA97KcvVH8kO+0nLwN\n" \
                              "4mGr/LxSBbAKas9S+dWMYrpmCJs3jIgi3rkJhY/N2j8VVUaPQQ0+renysy8mGaIH\n" \
                              "40s0/LECgYB5mrs6IpDpfd4gpgp/PQFv51qZyEGczots9/NceHpZvuEuRaRDFYFK\n" \
                              "UfPiikAkDsfaVDhkKr5y9+UAdO1gwIVBR1rmz2I65xYGxCR8fkZoFPD+hEnF/6IS\n" \
                              "BzMAUz1b5VCgD57kp3Zj7ZxovF5XR4uHzuFrpApLg4Xj+vdkucWcDQ==\n" \
                              "-----END RSA PRIVATE KEY-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);


int LDRPin = 13;  //number of the pin connected to the digital output of the sensor
int PIRPin = 23;  //number of the pin connected to the digital output of the sensor
int LedPin = 21;
int temp = 14;
int fan = 12;

void setup()
{
  pinMode(LDRPin, INPUT);
  pinMode(PIRPin, INPUT);
  pinMode(LedPin, OUTPUT);
  pinMode(temp, INPUT);
  pinMode(fan, OUTPUT);
  digitalWrite(fan, LOW);
  digitalWrite(LedPin, LOW);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  espClient.setCACert(test_root_ca);
  espClient.setCertificate(test_client_cert);
  espClient.setPrivateKey(test_client_key);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32_abribus1", mqttUser, mqttPassword )) {

      Serial.println("Connected to MQTT");
      client.subscribe("abribuses/abribus1");
    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    
    };
  }

}

void loop()
{
  client.loop();
  int temperature = analogRead(temp);
  int  LDR = digitalRead(LDRPin);
  int PIR = digitalRead(PIRPin);
  Serial.println("LDR");
  Serial.println(LDR);
  Serial.println("PIR");
  Serial.println(PIR);

  Serial.println("temperature");
  Serial.println(temperature);


if (PIR == HIGH){

Serial.println("Presence detected");
display.display();
if (LDR == HIGH) {
digitalWrite(LedPin, HIGH);
Serial.println("LED ON");
}
   else {
digitalWrite(LedPin, LOW);
   Serial.println("LED OFF");
  }
  int  Temp_seuil = 400; //en mV
  
  if ( temperature >= Temp_seuil){
  digitalWrite(fan, HIGH);
 Serial.println("FAN ON");
 }

  else {
  digitalWrite(fan, LOW);
  Serial.println("FAN OFF");

  };
  
  delay(500);
}
   else{
       Serial.println("Presence undetected");
       digitalWrite(fan, LOW);
      Serial.println("FAN OFF");
      digitalWrite(LedPin, LOW);
      
      Serial.println("LED OFF");
};
}


void callback(char* topic , byte* payload, unsigned int length) {
  char inData[80];
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);


  Serial.println("-------new message from broker-----");
  Serial.print("channel:");
  Serial.println(topic);
  Serial.print("data:");
  Serial.write(payload, length);
  Serial.println();

  Serial.println("Parsing start: ");

  StaticJsonBuffer<300> JSONBuffer;                         //Memory pool
  JsonObject& parsed = JSONBuffer.parseObject(payload); //Parse message

  if (!parsed.success()) {   //Check for errors in parsing

    Serial.println("Parsing failed");
    delay(5000);
    return;

  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("  Bus        Time ");


  String id1 = parsed["bus1"];
  int time1 = parsed["time1"];
  String id2 = parsed["bus2"];
  int time2 = parsed["time2"];

  String t1 = String(time1/60);
  String t2 = String(time2/60);

  String a = "   "+ id1 + "         " +t1+"min";
  String b = "   "+ id2 + "         " +t2+"min";
  String b2 = "   "+ id2 + "         " +"Arrived";
  if ( time2 > 15 ){
    if ( time2 > 45){
      display.println(b);
    }
    else{
      display.println(b2);
    }
  }

  display.println(a);
  display.display();
  Serial.println(a);

}
