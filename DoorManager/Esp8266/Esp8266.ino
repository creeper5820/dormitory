#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/// @brief MQTT相关变量声明
const char* ssid = "wifi410"; /*WIFI*/
const char* password = "18661205519"; /*KEYS*/
const char* mqttServer = "124.220.206.170"; /*公网ip*/
const char* MySub = "door410"; /*订阅主题*/
const char* username = "creeper"; /*mqtt用户名*/
const char* pwd = "58chenqiquan20"; /*mqtt密码*/

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
const byte ledPin = BUILTIN_LED;

/// @brief 初始化函数
void setup()
{
    Serial.begin(115200);

    // 初始化WIFI
    WiFi.mode(WIFI_STA);
    connectWifi();

    // 初始化MQTT服务器
    mqttClient.setServer(mqttServer, 1883);
    connectMQTTServer();
    mqttClient.setCallback(callback);

    // 板载灯初始化
    pinMode(LED_BUILTIN, OUTPUT);
    analogWriteRange(2000);
    analogWrite(LED_BUILTIN, 2000);

    // 信号引脚初始化
    pinMode(D1, OUTPUT);
}

/// @brief 主循环
void loop()
{
    if (mqttClient.connected())
        mqttClient.loop();
    else
        connectMQTTServer();

    analogWrite(LED_BUILTIN, 2000);
}

/// @brief 连接MQTT服务器
void connectMQTTServer()
{
    // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
    String clientId = "esp8266-" + WiFi.macAddress();
    // 连接MQTT服务器
    if (mqttClient.connect(clientId.c_str(), username, pwd)) { //设置mqtt主题的id
        //连接成功后就订阅主题
        mqttClient.subscribe(MySub); //订阅主题
        Serial.print("订阅主题成功！！");
    }
    else {
        Serial.print("Subscribe Fail...");
        delay(3000);
    }
}

/// @brief ESP8266连接WIFI
void connectWifi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi Connected!");
    Serial.println("");
}

/// @brief MQTT收到信息回调函数
void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]\n");

    for (int i = 0; i < length; i++)
        Serial.print((char)payload[i]);

    Serial.println();

    switch ((char)payload[0]) {
    case '1':
        Serial.print("开门");
        OpenDoor();
        break;
    case '2':
        Serial.print("回正");
        break;
    }
}

void OpenDoor()
{
    digitalWrite(D1, HIGH);
    delay(500);
    digitalWrite(D1, LOW);
}