#include <Arduino.h>
#include "src/CarmelaAction.h"


#define BUF_LEN           64
#define RES_LEN           30

#define MAX_PACKAGE_LEN   35
#define MIN_PACKAGE_LEN   8
#define BAUD              9600
#define PACKAGE_SLOT      5 //解析数据包间的时间间隔，单位ms
#define PACKAGE_HEADER    0x7E
#define PROTOCOL_VERSION  0x31
#define PACKAGE_TAIL      0x0D
#define ARDUINO_ADDRESS   0x01
#define PC_ADDRESS        0x00

#define PACKAGE_UP_REPORT    0x00
#define PACKAGE_DOWN_CMD     0xFF

//根据串口协议定义的命令
#define CARMELA_STOP      0x00
#define CARMELA_FORWARD   0x01
#define CARMELA_BACKWARD  0x02
#define CARMELA_RIGHT     0x03
#define CARMELA_LEFT      0x04
#define CARMELA_PID_XY    0x10
#define CARMELA_PID_NXY   0x11
#define CARMELA_PID_POLAR 0x20
#define CARMELA_PID_NPOLAR 0x21
#define CARMELA_PID_LEFT_ANGLE  0x30
#define CARMELA_PID_RIGHT_ANGLE  0x31
#define CARMELA_STOP_REPLY       0x40
#define CARMELA_REPLY_POSE       0x41
#define CARMELA_REPLY_RAWDATA    0x42
#define CARMELA_REPLY_BOTH       0x43

unsigned long previousMillis = 0; //当前时间
bool maxCommandReceived = false;//串口数组溢出标志位
char buff[BUF_LEN] = {};  //接收串口命令的数组
char buffIndex = 0;     //数组中数据的个数
unsigned int sampleInterval;//上报数据的时间间隔

void startParsePackage();//是否需要解析包
char findPackageHead(char bufflen); //找到数据包的头
bool parsePackage(char buffLen);//解析数据包的命令

void flagToStop();      //停止标志置1，其他清0
void flagToForward();   //前进标志置1，其他清0
void flagToBackward();  //后退标志置1，其他清0
void flagToRight();     //右转标志置1，其他清0
void flagToLeft();      //左转标志置1，其他清0
void flagToPIDXY();     //使用PID到指定坐标点标志置1，其他清0
void flagToPIDNXY();    //使用PID到N个指定坐标点标志置1，其他清0
void flagToPIDPolar();  //使用PID到极坐标点标志置1，其他清0
void flagToPIDNPolar(); //使用PID到N个极坐标点标志置1，其他清0
void flagToLeftAngle(); //左转固定角度标志置1，其他清0
void flagToRightAngle();//右转固定角度标志置1，其他清0
void flagToStopReply(); //停止上传信息标志置1，其他清0
void flagToReplyPose(); //仅上传位姿信息标志置1，其他清0
void flagToReplyRAWDATA();//仅上传编码器数据标志置1，其他清0
void flagToReplyBoth();   //位姿和编码器数据一起上传标志置1，其他清0

/********flagToStop()**************
函数功能：下位机停止标志位置位
输入参数：无
输出参数：无
***********************************/
void flagToStop()
{
  flagCarmelaStop = true;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToForward()***********
函数功能：下位机前进标志位置位
输入参数：无
输出参数：无
***********************************/
void flagToForward()
{
  flagCarmelaStop = false;
  flagCarmelaForward = true;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToBackward()**********
函数功能：下位机后退标志位置位
输入参数：无
输出参数：无
***********************************/
void flagToBackward()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = true;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToRight()**************
函数功能：下位机右转标志位置位
输入参数：无
输出参数：无
***********************************/
void flagToRight()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = true;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToLeft()**************
函数功能：下位机左转标志位置位
输入参数：无
输出参数：无
***********************************/
void flagToLeft()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = true;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToPIDXY()****************
函数功能：下位机PID方式到指定坐标位置
            标志位置位
输入参数：无
输出参数：无
*************************************/
void flagToPIDXY()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = true;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToPIDNXY()**************
函数功能：下位机PID方式连续到N个
            指定坐标位置标志位置位
输入参数：无
输出参数：无
***********************************/
void flagToPIDNXY()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = true;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToPIDPolar()**************
函数功能：下位机PID方式到指定极坐标位置
            标志位置位
输入参数：无
输出参数：无
***************************************/
void flagToPIDPolar()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = true;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToPIDNPolar()**************
函数功能：下位机PID方式到N个指定极坐标位置
            标志位置位
输入参数：无
输出参数：无
***************************************/
void flagToPIDNPolar()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = true;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToLeftAngle()**************
函数功能：下位机左转指定角度标志位置位
输入参数：无
输出参数：无
***************************************/
void flagToLeftAngle()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = true;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
 // flagCarmelaReplyBoth = false;
}
/********flagToRightAngle()**************
函数功能：下位机右转指定角度标志位置位
输入参数：无
输出参数：无
***************************************/
void flagToRightAngle()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = true;
  flagCarmelaStopReply = false;
  //flagCarmelaReplyPose = false;
  //flagCarmelaReplyRAWDATA = false;
  //flagCarmelaReplyBoth = false;
}
/********flagToStopReply()**************
函数功能：下位机停止上报标志位置位
输入参数：无
输出参数：无
***************************************/
void flagToStopReply()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = true;
  flagCarmelaReplyPose = false;
  flagCarmelaReplyRAWDATA = false;
  flagCarmelaReplyBoth = false;
}
/********flagToReplyPose()**************
函数功能：下位机上报位姿标志位置位
输入参数：无
输出参数：无
***************************************/
void flagToReplyPose()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  flagCarmelaReplyPose = true;
  flagCarmelaReplyRAWDATA = false;
  flagCarmelaReplyBoth = false;
}
/********flagToReplyRAWDATA()**************
函数功能：下位机上报编码器数据标志位置位
输入参数：无
输出参数：无
***************************************/
void flagToReplyRAWDATA()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  flagCarmelaReplyPose = false;
  flagCarmelaReplyRAWDATA = true;
  flagCarmelaReplyBoth = false;
}
/********flagToLeftAngle()**************
函数功能：下位机上报位置和编码器数据
            标志位置位
输入参数：无
输出参数：无
***************************************/
void flagToReplyBoth()
{
  flagCarmelaStop = false;
  flagCarmelaForward = false;
  flagCarmelaBackward = false;
  flagCarmelaRight = false;
  flagCarmelaLeft = false;
  flagCarmelaPIDXY = false;
  flagCarmelaPIDNXY = false;
  flagCarmelaPIDPolar = false;
  flagCarmelaPIDNPolar = false;
  flagCarmelaLeftAngle = false;
  flagCarmelaRightAngle = false;
  flagCarmelaStopReply = false;
  flagCarmelaReplyPose = false;
  flagCarmelaReplyRAWDATA = false;
  flagCarmelaReplyBoth = true;
}

/*****char findPackageHead（char bufflen）******
函数功能：找到包头的位置
输入参数：收到命令的长度
返回参数: 包头的位置
***********************************************/
char findPackageHead(char bufflen)
{
  char offset = 0; //包头的位置

  do
  {
    if(PACKAGE_HEADER == buff[offset])
    {
      if(PROTOCOL_VERSION == buff[offset+1])
      {
        if(ARDUINO_ADDRESS == buff[offset+2])
        {
          if(PACKAGE_HEADER !=buff[offset+3]) // avoid 7E 31 01 7E 31 01 00 FF 00 31 0D
          {
            return offset;
          }
          else
          {
            offset += 2; //should +3 ,but offset++ below
          }
        }
      }
    }
    offset++;
  }while(offset <= bufflen - MIN_PACKAGE_LEN);

  return offset;
}

/*****bool parsePackage(char brffLen)******
函数功能：解析包的命令
输入参数：收到命令的长度
输出参数：解析命令正确返回1，不正确返回0
*********************************************/
bool parsePackage(char buffLen)
{
  char packageHead = 0;//包头位置
  char dataLen = 0;  //parameter length
  char checkSum = 0;

  packageHead = findPackageHead(buffLen);    //packageHead is the position
  if(packageHead > buffLen - MIN_PACKAGE_LEN ) // not found the head
  {
    return false;
  }
   //found the head
    dataLen = buff[packageHead+5];
    if(PACKAGE_TAIL != buff[packageHead+dataLen+7]) //the tail is not right, return false
    {
      return false;
    }
    //the tail is right
    for(char i = 1;i<=dataLen+5;i++)
    {
      checkSum += buff[packageHead+i];
    }
    if(checkSum != buff[packageHead + dataLen +6])
    {
      return false;
    }
    switch (buff[packageHead+3])     //analysis the CTF/RSF
    {
      case CARMELA_STOP:           flagToStop();         break;
      case CARMELA_FORWARD:        flagToForward();      break;
      case CARMELA_BACKWARD:       flagToBackward();     break;
      case CARMELA_RIGHT:          flagToRight();        break;
      case CARMELA_LEFT:           flagToLeft();         break;
      case CARMELA_PID_XY:         flagToPIDXY();        break;
      case CARMELA_PID_NXY:        flagToPIDNXY();       break;
      case CARMELA_PID_POLAR:      flagToPIDPolar();     break;
      case CARMELA_PID_NPOLAR:     flagToPIDNPolar();    break;
      case CARMELA_PID_LEFT_ANGLE: flagToLeftAngle();    break;
      case CARMELA_PID_RIGHT_ANGLE:flagToRightAngle();   break;
      case CARMELA_STOP_REPLY:     flagToStopReply();    break;//关闭上报
      case CARMELA_REPLY_POSE:     flagToReplyPose();
                                   if(0x01 == buff[5]){
                                      sampleInterval = buff[6]*100; //0.1秒为单位
                                   }
                                   else{
                                       return false;
                                   }
                                    break;

      case CARMELA_REPLY_RAWDATA:  flagToReplyRAWDATA();
                                   if(0x01 == buff[5]){
                                      sampleInterval = buff[6]*100; //0.1秒为单位
                                   }
                                   else{
                                       return false;
                                   }
                                    break;

      case CARMELA_REPLY_BOTH:     flagToReplyBoth();
                                   if(0x01 == buff[5]){
                                      sampleInterval = buff[6]*100; //0.1秒为单位
                                   }
                                   else{
                                       return false;
                                   }
                                    break;
      default: return false;
    }
    return true;
}
//----------------------------------------------------------------//

/*****void startParsePackage()******
函数功能：满足条件后，开始解析包，如果
        解析正确，则下位机执行相应的命令。
输入参数：无
输出参数：无
************************************/
void startParsePackage()
{
  if(buffIndex&&(millis()-previousMillis>PACKAGE_SLOT))
    //缓冲区有数据并且一帧数据发送完了
 {
    maxCommandReceived = false;//this
    if(parsePackage(buffIndex) == true) //解析包正确
    {
      CarmelaAction();
    }
    else
    {
      //解析错误
      Serial3.write(4);
    }
    buffIndex = 0; //解析完成后，数组中的数据清0
  }
}

/********Serial3 Interrupt*********************
函数功能：把上位机发来的数据存放到串口数组中
输入参数：无
输出参数：无
***********************************************/
void serialEvent3() //串口3使用蓝牙通信，串口0为串口通信
{
  while(Serial3.available())
  {
    buff[buffIndex++] = Serial3.read();
    if(buffIndex >= MAX_PACKAGE_LEN) //this ?
    {
      maxCommandReceived = true;
      Serial3.flush();
    }
  }
  previousMillis = millis(); //读取当前时间，去判断一组
                            //数据包是否发送完。
}
