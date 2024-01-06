#include <Arduino.h>
#include <Wifi.h>
#include <PubSubClient.h>

#include "wificonfig.h"

// #region 通用配置区
const char *ssid = "embedLab";
const char *mqtt_server = "192.168.1.200";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
// #endregion

// TODO #region 个性化配置
#define SWGroup1
// #define SWGroup2
// #define SWGroup3

const char *cmdTopic = "character/all";
#ifdef SWGroup1
const char *reportSwTopic = "report/character1";
#endif
#ifdef SWGroup2
const char *reportSwTopic = "report/character2";
#endif
#ifdef SWGroup3
const char *reportSwTopic = "report/character3";
#endif
const char *reportTopic = "report/all";
String clientId(reportSwTopic);
// #endregion

// TODO #region 本地离线逻辑

const int led1Pin = 33;
const int sw1Pin = 13;

const int led2Pin = 12;
const int sw2Pin = 14;

const int led3Pin = 26;
const int sw3Pin = 27;

// 连接wifi过程
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void correct()
{
  // 正确答案，全部亮起来
  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led3Pin, HIGH);
}
void wrong()
{
  // 错误答案，闪烁
  for (size_t i = 0; i < 5; i++)
  {
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);
    digitalWrite(led3Pin, HIGH);
    delay(500);
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, LOW);
    digitalWrite(led3Pin, LOW);
    delay(500);
  }
}

// 收到mqtt订阅回调
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();
  // payload转成char数组
  char cmd[length + 1];
  cmd[length] = '\0';
  for (int i = 0; i < length; i++)
  {
    cmd[i] = (char)payload[i];
    Serial.print(cmd[i]);
  }
  // char数组再转成String
  String cmdstr(cmd);
  String topicstr(topic);
  String mcd(cmdTopic);

  // TODO =================主逻辑=================
  if (topicstr == mcd)
  {
    Serial.println("ReceiveMyCmd!");
    Serial.println(cmdstr);
    if (cmdstr == "ISONLINE")
    {
#ifdef SWGroup1
      client.publish(reportTopic, "character/1: live");
#endif
#ifdef SWGroup2
      client.publish(reportTopic, "character/2: live");
#endif
#ifdef SWGroup3
      client.publish(reportTopic, "character/3: live");
#endif
    }

    if(cmdstr=="correct")
    {
      correct();
    }
    if(cmdstr=="wrong")
    {
      wrong();
    }

#ifdef SWGroup1
    if (cmdstr == "1on")
    {
      client.publish(reportTopic, "character/1: 1on");
      digitalWrite(led1Pin, HIGH);
    }
    if (cmdstr == "1off")
    {
      client.publish(reportTopic, "character/1: 1off");
      digitalWrite(led1Pin, LOW);
    }
    if (cmdstr == "2on")
    {
      client.publish(reportTopic, "character/1: 2on");
      digitalWrite(led2Pin, HIGH);
    }
    if (cmdstr == "2off")
    {
      client.publish(reportTopic, "character/1: 2off");
      digitalWrite(led2Pin, LOW);
    }

    if (cmdstr == "3on")
    {
      client.publish(reportTopic, "character/1: 3on");
      digitalWrite(led3Pin, HIGH);
    }
    if (cmdstr == "3off")
    {
      client.publish(reportTopic, "character/1: 3off");
      digitalWrite(led3Pin, LOW);
    }
#endif
#ifdef SWGroup2
    if (cmdstr == "4on")
    {
      client.publish(reportTopic, "character/2: 4on");
      digitalWrite(led1Pin, HIGH);
    }
    if (cmdstr == "4off")
    {
      client.publish(reportTopic, "character/2: 4off");
      digitalWrite(led1Pin, LOW);
    }
    if (cmdstr == "5on")
    {
      client.publish(reportTopic, "character/2: 5on");
      digitalWrite(led2Pin, HIGH);
    }
    if (cmdstr == "5off")
    {
      client.publish(reportTopic, "character/2: 5off");
      digitalWrite(led2Pin, LOW);
    }

#endif

#ifdef SWGroup3
    if (cmdstr == "6on")
    {
      client.publish(reportTopic, "character/3: 6on");
      digitalWrite(led1Pin, HIGH);
    }
    if (cmdstr == "6off")
    {
      client.publish(reportTopic, "character/3: 6off");
      digitalWrite(led1Pin, LOW);
    }
    if (cmdstr == "7on")
    {
      client.publish(reportTopic, "character/3: 7on");
      digitalWrite(led2Pin, HIGH);
    }
    if (cmdstr == "7off")
    {
      client.publish(reportTopic, "character/3: 7off");
      digitalWrite(led2Pin, LOW);
    }
    if (cmdstr == "8on")
    {
      client.publish(reportTopic, "character/3: 8on");
      digitalWrite(led3Pin, HIGH);
    }
    if (cmdstr == "8off")
    {
      client.publish(reportTopic, "character/3: 8off");
      digitalWrite(led3Pin, LOW);
    }

#endif
  }
  // ======================================
}

// 和MQTT服务器断连后自动重连
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(cmdTopic);
      // 上报上线消息
#ifdef SWGroup1
      client.publish(reportTopic, "character/1: online"); // TODO 改名字
#endif
#ifdef SWGroup2
      client.publish(reportTopic, "character/2: online"); // TODO 改名字
#endif
#ifdef SWGroup3
      client.publish(reportTopic, "character/3: online"); // TODO 改名字
#endif
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // 初始化串口
  Serial.begin(115200);

  setup_wifi();

  // espClient.setFingerprint(fingerprint);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // TODO #region 本地离线逻辑
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(sw1Pin, INPUT_PULLUP);
  pinMode(sw2Pin, INPUT_PULLUP);
  pinMode(sw3Pin, INPUT_PULLUP);
}
void blinkLed(int pin)
{
  digitalWrite(pin, HIGH);
  delay(1500);
  digitalWrite(pin, LOW);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (digitalRead(sw1Pin) == LOW)
  {
    client.publish(reportSwTopic, "sw1");
    delay(100);
  }
  if (digitalRead(sw2Pin) == LOW)
  {
    client.publish(reportSwTopic, "sw2");
    delay(100);
  }
  if (digitalRead(sw3Pin) == LOW)
  {
    client.publish(reportSwTopic, "sw3");
    delay(100);
  }

  // // 读取震动传感器
  // for (size_t i = 0; i < 5; i++)
  // {
  //   if (digitalRead(vibratePinList[i]) == HIGH)
  //   {
  //     ledTimer[i]=millis()+1500;

  //     *(vmsg+11)='1'+i;
  //     client.publish(reportTopic, vmsg);

  //     play(i+1);
  //     delay(100);
  //   }
  // }
  // // Serial.println(analogRead(vibratePinList[0]));
  // // LED 逻辑
  // for (size_t i = 0; i < 5; i++)
  // {
  //   if (ledTimer[i]>millis())
  //   {
  //     digitalWrite(ledPinList[i], HIGH);
  //   }
  //   else
  //   {
  //     digitalWrite(ledPinList[i], LOW);
  //   }
  // }
}
