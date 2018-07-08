#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

const char ssid[] = "Xiaomi_1111";
const char pass[] = "20001937";

WiFiClientSecure net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // MQTT brokers usually use port 8443 for secure connections.
  client.begin("192.168.31.84", 8443, net);

  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every 3 second.
  if (millis() - lastMillis > 30000) {
    lastMillis = millis();

    //上传数据到Mongodb数据库
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["db_name"] = String("uploads");//coolpy7 dbpoxy对应的数据库名
    root["table_name"] = String("datapoints");//coolpy7 dbpoxy对应的数据表名
    root["op_name"] = String("insert");//支持insert,update,delete,query等操作符
    root["token"] = String("password");
    root["value"] = RawJson("{\"dp\":10.12}"); //准备上传的数据结点值，假设实际应用时上传温湿度值使用此项上传
    String output;
    root.printTo(output);

    client.publish("dbpoxy/mongodb/get", output.c_str());
  }
}
