#include "NFC_Controller.hh"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <Adafruit_Fingerprint.h>

/**
 * 舵机驱动相关变量声明
*/
Servo myservo; //定义舵机

int servo_max = 180;
int servo_close = 0;

/**
 * MQTT模块相关变量声明
*/
const char* ssid = "wifi410"; /*WIFI*/
const char* password = "18661205519"; /*KEYS*/
const char* mqttServer = "124.220.206.170"; /*公网ip*/
const char* MySub = "door410"; /*订阅主题*/
const char* username = "creeper"; /*mqtt用户名*/
const char* pwd = "58chenqiquan20"; /*mqtt密码*/

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
const byte ledPin = BUILTIN_LED;

/**
 * NFC驱动类声明
*/
auto nfc_controller = NFC_Controller();

void setup()
{
    Serial.begin(115200);

    // 初始化NFC驱动
    nfc_controller.NFC_Init();
    nfc_controller.Set_User_ID(User_ID{ 50, 126, 106, 83 }, 0);

    // 初始化WIFI
    WiFi.mode(WIFI_STA);
    connectWifi();

    // 初始化MQTT服务器
    mqttClient.setServer(mqttServer, 1883);
    connectMQTTServer();
    mqttClient.setCallback(callback);

    // 初始化舵机
    myservo.attach(D2);
    myservo.write(90);

    // 板载灯初始化
    pinMode(LED_BUILTIN, OUTPUT);
    analogWriteRange(2000);
    analogWrite(LED_BUILTIN, 2000);

    //蜂鸣器I/O口初始化
    pinMode(D4, OUTPUT);
    digitalWrite(D4, LOW);
}

void loop()
{
    if (mqttClient.connected())
        mqttClient.loop();
    else
        connectMQTTServer();

    if (nfc_controller.Find_Card())
        if (nfc_controller.Identify_All()) {
            Serial.println("Hello World");
            IdentfyPass();
            open_door();
        }
        else {
            IdentfyNotPass();
            Serial.println("你是谁啊??????????");
        }
}

/**
 * @brief 开门的函数
*/
void open_door()
{

    analogWrite(LED_BUILTIN, 0);
    //正转
    myservo.write(0); //正向最大速度
    delay(1400); //持续时间
    myservo.write(90); //停止

    //反转
    myservo.write(180); //反向最大速度
    delay(1000);
    myservo.write(90); //停止
}
//回正按钮
void rollback()
{
    analogWrite(LED_BUILTIN, 0);

    analogWrite(LED_BUILTIN, 0);
    myservo.write(0); //正向最大速度
    delay(200);
    myservo.write(90); //停止
    analogWrite(LED_BUILTIN, 2000);
}

/**
 * @brief 连接MQTT服务器
*/
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

/**
 * @brief ESP8266连接WIFI
*/
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

/**
 * @brief MQTT收到信息回调函数
*/
void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]\n");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    switch ((char)payload[0]) {
    case '1':
        Serial.print("开门");
        IdentfyPass();
        open_door();
        break;
    case '2':
        rollback();
        Serial.print("回正");
        break;
    }
}

/**
 * @brief 蜂鸣器瞎叫函数
*/
void IdentfyPass()
{
    digitalWrite(D4, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(1000); // wait for a second
    digitalWrite(D4, LOW); // turn the LED off by making the voltage LOW
}

void IdentfyNotPass()
{
    digitalWrite(D4, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(100); // wait for a second
    digitalWrite(D4, LOW); // turn the LED off by making the voltage LOW

    delay(100);

    digitalWrite(D4, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(100); // wait for a second
    digitalWrite(D4, LOW); // turn the LED off by making the voltage LOW

    delay(100);

    digitalWrite(D4, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(100); // wait for a second
    digitalWrite(D4, LOW); // turn the LED off by making the voltage LOW
}