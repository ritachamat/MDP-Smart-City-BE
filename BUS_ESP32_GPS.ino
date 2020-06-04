#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>    

const char* ssid = "ssid";
const char* password =  "password";  //
const char* mqttServer = "mqtt ID";
const int mqttPort = 8883;
const char* mqttUser = "bus1";
const char* mqttPassword = "pass";

const char* test_root_ca= \
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
"MIIDpzCCAo8CFDT56Y/RvISP9JOjQVM0hhtGLgeeMA0GCSqGSIb3DQEBCwUAMIGU\n" \
"MQswCQYDVQQGEwJMQjEQMA4GA1UECAwHTGViYW5vbjEPMA0GA1UEBwwGQmVpcnV0\n" \
"MQ0wCwYDVQQKDARwcm9qMQwwCgYDVQQLDANtZHAxHTAbBgNVBAMMFHByb2pldDA0\n" \
"LmR1Y2tkbnMub3JnMSYwJAYJKoZIhvcNAQkBFhdyaXRhLmNoYW1hdEBob3RtYWls\n" \
"LmNvbTAeFw0yMDA1MTkxNDA0MTdaFw0yMzAyMTMxNDA0MTdaMIGKMQswCQYDVQQG\n" \
"EwJMQjERMA8GA1UECAwITGViYW5vbiAxDzANBgNVBAcMBkJlaXJ1dDENMAsGA1UE\n" \
"CgwEcHJvajEMMAoGA1UECwwDbWRwMRIwEAYDVQQDDAlsb2NhbGhvc3QxJjAkBgkq\n" \
"hkiG9w0BCQEWF3JpdGEuY2hhbWF0QGhvdG1haWwuY29tMIIBIjANBgkqhkiG9w0B\n" \
"AQEFAAOCAQ8AMIIBCgKCAQEAxxwmXx4xTPYo7p1K7Y9Ebmv88WHu4kOCPLqdBtFZ\n" \
"FmM3uY1fTn4zZhXrob9vKsuZo0FLGXfAsrmXUWStalfwgLCHTBUgai63N/GhgB3K\n" \
"PKT+6GAw0RtiuSOMAPxhPeCvceQFx6sUEymYp2YjqeazpWNpo87sNTE+tgobMWo9\n" \
"WnjqA+FXoO3Y4UpISJ7uJiyFv5NlmrsNiXq5TV/hPrhYeRcrLNQhKSMMBrrKi4s2\n" \
"6gPEtF6ZmEpwIn/K8m9aYOOp2yBrf39oarNMSM3sEB/RRocQYT1HIbrBtoQDe9wo\n" \
"74D2i2V9u+UaHa+Rr5IMaYI/3XpaaWCnNosJ1+Zt0o+HYQIDAQABMA0GCSqGSIb3\n" \
"DQEBCwUAA4IBAQCZdfNirzN+8kOsWkMkovpu7KsRw0n9d6HyK6CEUNEZoPRxo6t7\n" \
"vZ9e4cIFL0ggWKVf8qNm8v2QNOv+EH9lCoQ/evWbpPsM3AUZLjM2vAjmMlPFvw2P\n" \
"fcWu2TtMPzHFBUr0vJf0otpuOnFCIf03wOQ/gaEpC2/KDaQMBASvxaVMGRACRxA+\n" \
"eyql2i3ZwdzKXqfCHkBQjn7DPEXYfFffZw3KVvJE2frCxFumOMDEx2gfe80R3UKO\n" \
"nqf/Yc0F9XkckCRF+BeZy0gl8hk9rmLhFm/hGpAjA+qTqp9x+wUPI7S2axVw34ns\n" \
"v/q9VPZegxhoL2VLRkLJXSj88LG4KDRiZcJ2\n" \
"-----END CERTIFICATE-----\n";

const char* test_client_key = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpgIBAAKCAQEAxxwmXx4xTPYo7p1K7Y9Ebmv88WHu4kOCPLqdBtFZFmM3uY1f\n" \
"Tn4zZhXrob9vKsuZo0FLGXfAsrmXUWStalfwgLCHTBUgai63N/GhgB3KPKT+6GAw\n" \
"0RtiuSOMAPxhPeCvceQFx6sUEymYp2YjqeazpWNpo87sNTE+tgobMWo9WnjqA+FX\n" \
"oO3Y4UpISJ7uJiyFv5NlmrsNiXq5TV/hPrhYeRcrLNQhKSMMBrrKi4s26gPEtF6Z\n" \
"mEpwIn/K8m9aYOOp2yBrf39oarNMSM3sEB/RRocQYT1HIbrBtoQDe9wo74D2i2V9\n" \
"u+UaHa+Rr5IMaYI/3XpaaWCnNosJ1+Zt0o+HYQIDAQABAoIBAQCRrzckQaMw/KvW\n" \
"a6B3UWxwnvLkaxOu2lZguOpMIsFccBxF1/r0I8LX3xIZt0hPt8kiepPIfqawHrtp\n" \
"ApEgTlfa0U8tjAe7qZLFjDbu1t0yokchH8CYP8FsVWYLEp7T96JnYsAX6EU4ACeB\n" \
"1Goj1nl7edHB2ZVbFj0cVSiwb1uU5bjT+Nmq27F6f/+sEkNSKawY9FLhYJBooBLi\n" \
"iyozds19rmXFfvCLnAR6NxlLsh+EXkyAlWWzaV1Lzb3rwMKHNmBiZ7f7xKsL9Rnc\n" \
"M/M8tvCcsZ/XhGOn45CTZ/REs7G8dQjOYA2MlGsRfpYqBtu5cT/gYlWM0yhkVzCT\n" \
"WM3/2X8BAoGBAO/1S6tfT4EbEV6lrLcMfAwXRkYGxQbK9Rk5Qko677ZLaYCteqHF\n" \
"dnwVpYMDR8byxYKge4ie3O3Cu9EGnYz/vI2i1eWYZrlUmnIjvcnBk3hNrAjTRq/P\n" \
"1PfBMWKI57fBc7s6ZBRIWGvzLHnIYeJnZbtcNg/xuFJgL+D6BVuti4UVAoGBANRr\n" \
"xH61VRK5D1hGV4ZuRRz2zIFEPzh+im7HKehXO4RCB63O2WwzAPSd/ooO/kYTcWtX\n" \
"RbEAhcOxHFUu4VuPJVx+So2SYP9KixyCGIIOxYHIaUUtlpsvAhpM1Nnw8p0rc/Zd\n" \
"g8henIcDQNpbCChMW0ApRg2RfjEtfEafIbnGgKQdAoGBAMRHCjFQkfDSGkAoFoHY\n" \
"VXNyakoHaYxn4VpTitS7Jq8fOlpGDKRcp9FRx4PF4CVtGYADE2kqQR6+hfCxj5fw\n" \
"6+OTCZF3Ordn67lnYOuySpVCO3EwmIZg4xwRS/sEbz3QoIDKf2hT0Pfj/kJTVTXj\n" \
"S8SY5aPfHd1Lzn4hJNbbut8NAoGBAJ4FIWKlIIlSE8E2asv9Yfqe+hk7nNA5CmBI\n" \
"VruZkRPdTvsUQpgEBTT0B7PKXYvh0ODqRlTbL6cZLRuNSWYkQcXhyTIx72hamsv1\n" \
"V6gAtxrJ7M3ikQQWXzkTaNBMNx+GgLiIbo1zHHwvhmIpkBLyYnuDeLKFRlvtG2Mp\n" \
"5BvgODhdAoGBALS4/Eu7hiPXsOOBnXDpufIcIamFyq4SEidbnBBuPQ2OJQStzXAY\n" \
"NeW3Ig8mSHp2a9IjZzvjjsfFeBkEctYyDx/xbO8G3VpnsMqNY1Xds1ngVdb7sl51\n" \
"UYY9D+2MyPdAKsfzAu4PKZDS7ap79Cp3OoAuAxsDALNcQmWYX3akObep\n" \
"-----END RSA PRIVATE KEY-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);
TinyGPSPlus gps;                                           

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 12, 15);
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
  
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }
  
}
void publishing(){
  //Serial.println("function publishing");
  StaticJsonDocument<200> doc;
  doc["bus"] = "2";
  String longitude = String(gps.location.lng(),5);
  String latitude = String(gps.location.lat(),5);
  String coordonnees = latitude+","+longitude;
  doc["coord"]= coordonnees;
  serializeJson(doc, Serial);
  Serial.println();
  //serializeJsonPretty(doc, Serial);
  char buffer[512];
  serializeJson(doc, buffer);
  client.publish("buses/bus1", buffer);
  smartDelay(3000);                                      

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}
void loop() {
  client.loop();
  publishing();
  if (!client.connected()){
    client.connect("ESP32_bus1", mqttUser, mqttPassword );
    Serial.print("reconnected to MQTT");
  }
  delay(10000);
}
