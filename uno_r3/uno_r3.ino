#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Servo.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *laser = AFMS.getMotor(1);  //激光器
Adafruit_DCMotor *mM1 = AFMS.getMotor(3);  //DC电机1
Adafruit_DCMotor *mM2 = AFMS.getMotor(4);  //DC电机2

Servo servo_Y,servo_X;

String inputString = "";  //串口指令暂存
bool stringComplete = false;  //指令传输完成标志
unsigned long lastTime=0;  //储存上次收到指令的时间
unsigned long lastSendVoltTime=0; //储存上次发送电压时间
unsigned long lastLasing=0;  //储存上次收到激光发射指令时间
int voltValue;  //储存测量的电池供电电压

void setup() {
  Serial.begin(115200);  //串口与树莓派ZERO W通信接收控制指令

  pinMode(15,INPUT);
  
  AFMS.begin();  //电机驱动板初始化

  servo_Y.attach(10);    //舵机1-Y轴占用10脚
  servo_X.attach(9);    //舵机2-X轴占用9脚
  
  servo_Y.write(110);   //初始化舵机角度，回中X轴75°，Y轴110，Y轴极限33°-160°，X轴无限制
  servo_X.write(75);
}

void loop() {
  unsigned long currentTime=millis();
  
  if(currentTime-lastTime>=90){    //规定90毫秒未收到指令，电机停止，防断线失控 
    mM1->run(RELEASE);
    mM2->run(RELEASE);
  }

  if(currentTime-lastLasing>=50){  //超过50毫秒未收到指令，激光停止
    laser->run(RELEASE);
    laser->setSpeed(0);
  }

  if(currentTime-lastSendVoltTime>=1000){   //每1秒发送一次供电电压（原始测量值)
    voltValue=analogRead(15);
    Serial.println(voltValue);
    lastSendVoltTime=millis();
  }
      
  if (stringComplete) {   
    if(inputString.startsWith("#")){    //DC电机控制指令格式“#F220B110"，F前进，B后退，R停止
      
      mM1->setSpeed(inputString.substring(2,5).toInt());  //分离指令中的速度参数
      mM2->setSpeed(inputString.substring(6,9).toInt());
            
      switch(inputString[1]){       //获取指令中的方向参数
        case 'F': mM1->run(FORWARD);break;
        case 'B': mM1->run(BACKWARD);break;
        case 'R': mM1->run(RELEASE);break;
      }
      switch(inputString[5]){
        case 'F': mM2->run(FORWARD);break;
        case 'B': mM2->run(BACKWARD);break;
        case 'R': mM2->run(RELEASE);break;        
      }
      lastTime=millis();  //储存最后收到指令时间

    }else if(inputString.startsWith("$")){     //舵机控制指令格式“$090090”
        servo_Y.write(inputString.substring(1,4).toInt());
        servo_X.write(inputString.substring(4,7).toInt());

    }else if(inputString.startsWith("*")){  //激光指令示例“ *!1 ”
        if(inputString.substring(2,3)=="1"){       //1低功率
          laser->setSpeed(20);
          laser->run(FORWARD);
        }else if(inputString.substring(2,3)=="2"){  //2全负荷
          laser->setSpeed(255);
          laser->run(FORWARD);
        }else if(inputString.substring(2,3)=="3"){   //3停止
          laser->run(RELEASE);
          laser->setSpeed(0);
        }
        lastLasing=millis();
    }

    inputString = "";  //清空暂存指令
    stringComplete = false;
  }
}

//串口中断程序
void serialEvent() {
  while(Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
