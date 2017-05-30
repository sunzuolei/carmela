#define BAUD_MOTOR    9600
#define BAUD_WIFI    115200
#define BUFF_LEN           64
#define STOP      0
#define FORWARD   1
#define BACKWARD  2
#define RIGHT     3
#define LEFT      4

char buff[BUFF_LEN] = {};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_WIFI);
  Serial1.begin(BAUD_MOTOR);
}

void loop() {
  // put your main code here, to run repeatedly:  
  String comData = "";
  
  while (Serial.available() > 0) {
    comData += char(Serial.read());
    delay(2);
  }

  if (comData.length()) {
    for (int i = 0; i < comData.length() ; i++) {
      switch (int(comData[i])-48) {
        case STOP:
          MoveStop();
          break;
        case FORWARD:
          MoveForward();
          break;
        case BACKWARD:
          MoveBackward();
          break;
        case RIGHT:
          MoveRight();
          break;
        case LEFT:
          MoveLeft();
          break;
        default:
          MoveStop();
          break;
      }
    }
  }
}

void MoveStop()
{
  int i,j;
  //Packetized serial mode
  //0xAA (该模式第一个字符)
  //0x82  我们设置的地址是130 = 0x82
  //0x00 命令：a中00向前01向后；b中04向前05向后
  //0x00 速度值，范围是0x00 - 0x80
  //0x02 校验位 = (0x82+0x00+0x00)&0x80
  char a[5]={0xAA,0x82,0x00,0x00,0x02};
  char b[5]={0xAA,0x82,0x04,0x00,0x06};

  for(i=0;i<5;i++)//通过串口1来控制电机驱动板
  {
    Serial1.print(a[i]);
  }
  for(j=0;j<5;j++)
  {
    Serial1.print(b[j]);
  }
}

void MoveForward()
{
  int i,j;
  char a[5]={0xAA,0x82,0x00,0x30,0x32};
  char b[5]={0xAA,0x82,0x04,0x30,0x36};
  for(i=0;i<5;i++) //通过串口1来控制电机驱动板
  {
    Serial1.print(a[i]);
  }
  for(j=0;j<5;j++)
  {
    Serial1.print(b[j]);
  }
}

void MoveBackward()
{
  int i,j;
  char a[5]={0xAA,0x82,0x01,0x30,0x33};
  char b[5]={0xAA,0x82,0x05,0x30,0x37};
  for(i=0;i<5;i++)//通过串口1来控制电机驱动板
  {
    Serial1.print(a[i]);
  }
  for(j=0;j<5;j++)
  {
    Serial1.print(b[j]);
  }
}

void MoveRight()
{
  int i,j;
  char a[5]={0xAA,0x82,0x00,0x30,0x32};
  char b[5]={0xAA,0x82,0x05,0x30,0x37};
  for(i=0;i<5;i++)//通过串口1来控制电机驱动板
  {
    Serial1.print(a[i]);
  }
  for(j=0;j<5;j++)
  {
    Serial1.print(b[j]);
  }
}

void MoveLeft()
{
  int i,j;
  char a[5]={0xAA,0x82,0x01,0x30,0x33};
  char b[5]={0xAA,0x82,0x04,0x30,0x36};
  for(i=0;i<5;i++)//通过串口1来控制电机驱动板
  {
    Serial1.print(a[i]);
  }
  for(j=0;j<5;j++)
  {
    Serial1.print(b[j]);
  }
}

