#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

const char ssid[] = "Xiaomi_1111"; //wifi信号名
const char pass[] = "20001937"; //wifi密码

WiFiClientSecure net;
MQTTClient client;

const int  ledPin = D0;

//测试本示例可以通过其他mqtt客户端发送消息到device/senser主题
//消息内容为: {"cmd":1} 值为1即led点亮，值为0即led关闭

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

  //注册订阅主题用以收接控制指令
  client.subscribe("device/senser");
  Serial.println("\sub topic device/senser !");
  //取消订阅
  //client.unsubscribe("device/senser");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  long cmd = root["cmd"];
  if (cmd == 1) {
    digitalWrite(ledPin, LOW);
  }else{
    digitalWrite(ledPin, HIGH);
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // 连接到Coolpy7服务器使用Tls安全通信通道
  client.begin("192.168.31.84", 8443, net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
}
