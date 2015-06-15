#include <Arduino.h>


boolean flagCarmelaStop;
boolean flagCarmelaForward;
boolean flagCarmelaBackward;
boolean flagCarmelaRight;
boolean flagCarmelaLeft;
boolean flagCarmelaPIDXY;
boolean flagCarmelaPIDNXY;
boolean flagCarmelaPIDPolar;
boolean flagCarmelaPIDNPolar;
boolean flagCarmelaLeftAngle;
boolean flagCarmelaRightAngle;
boolean flagCarmelaStopReply;
boolean flagCarmelaReplyPose;
boolean flagCarmelaReplyRAWDATA;
boolean flagCarmelaReplyBoth;

void CarmelaStop()
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

void CarmelaForward()
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

void CarmelaBackward()
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

void CarmelaRight()
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

void CarmelaLeft()
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

void CarmelaPIDXY()
{
  Serial3.println("Carmela PIDXY!");
}

void CarmelaPIDNXY()
{
  Serial3.println("Carmela PIDNXY!");
}

void CarmelaPIDPolar()
{
  Serial3.println("Carmela PIDPolar!");
}

void CarmelaPIDNPolar()
{
  Serial3.println("Carmela PIDNPolar!");
}

void CarmelaLeftAngle()
{
  Serial3.println("Carmela LeftAngle!");
}

void CarmelaRightAngle()
{
  Serial3.println("Carmela RightAngle!");
}

void CarmelaStopReply()
{
  flagCarmelaReplyPose = 0;
  flagCarmelaReplyRAWDATA = 0;
  flagCarmelaReplyBoth = 0;

  Serial3.println("Carmela StopReply!");
}

/***********CarmelaAction()***********
函数功能：下位机根据标志位来响应的动作
输入参数：无
输出参数：无
*************************************/
void CarmelaAction()
{
  if(flagCarmelaStop)
  {
    CarmelaStop();
  }

  if(flagCarmelaForward)
  {
    CarmelaForward();
  }

  if(flagCarmelaBackward)
  {
    CarmelaBackward();
  }

  if(flagCarmelaRight)
  {
    CarmelaRight();
  }

  if(flagCarmelaLeft)
  {
    CarmelaLeft();
  }

  if(flagCarmelaPIDXY)
  {
    CarmelaPIDXY();
  }

  if(flagCarmelaPIDNXY)
  {
    CarmelaPIDNXY();
  }

  if(flagCarmelaPIDPolar)
  {
    CarmelaPIDPolar();
  }

  if(flagCarmelaPIDNPolar)
  {
    CarmelaPIDNPolar();
  }

  if(flagCarmelaLeftAngle)
  {
    CarmelaLeftAngle();
  }

  if(flagCarmelaRightAngle)
  {
    CarmelaRightAngle();
  }
    if(flagCarmelaStopReply)
  {
    CarmelaStopReply();
  }

}



