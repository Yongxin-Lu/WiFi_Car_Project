#include <ESP8266WiFi.h>

#define STASSID "@PHICOMM_00"    //路由器热点参数
#define STAPSK  "1234567890"

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.2.122";  //TCP服务器参数
const uint16_t port = 8234;

String dcMotorCom="#";  //控制指令缓存变量
String svMotorCom="$";

WiFiClient client;

void setup() {
  Serial.begin(115200);   //初始化串口并与热点和TCP服务器建立连接

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { //等待WIFI连接成功
    delay(500);
  }

  client.connect(host, port);
}

void loop() { 
  char temp=(char)client.read();
  if(temp=='#'){   //检查收到的TCP消息并串口转发,DC电机控制指令’#‘号开始
    for(int i=0;i<8;i++){
      char tempchar=(char)client.read();
      dcMotorCom += tempchar;
    }
    Serial.println(dcMotorCom);
    dcMotorCom="#";
    
  }else if(temp=='$'){  //舵机控制指令‘$’号开始
    for(int i=0;i<6;i++){
      char tempchar=(char)client.read();
      svMotorCom += tempchar;
    }
    Serial.println(svMotorCom);
    svMotorCom="$";
  }
}
