#include <Arduino.h>

//定义读取编码器的管脚
#define LEFT_FRONT_B   5 // General pin
#define LEFT_REAR_B    6
#define RIGHT_FRONT_B  7
#define RIGHT_REAR_B   8
#define ENCODING_SLOT  2  //编码器读数时防杂波


#define CARMELA_L   18   //小车长度Cm
#define CARMELA_B   28  //小车宽度Cm
#define CARMELA_C   40    //小车轮子的周长Cm
#define CARMELA_P   700   // = 43.7*16  (既m*n*w)

//四个轮子编码器的计数
long rightfrontEncoderCnt = 0;
long rightrearEncoderCnt = 0;
long leftrearEncoderCnt = 0;
long leftfrontEncoderCnt = 0;

//位姿解算中的左右转、前进标志
bool flagMotionForward = false;
bool flagMotionRight = false;
bool flagMotionLeft = false;


/***motionModel(long leftCnt,long rightCnt)***
函数功能：由编码器数据计算走的距离和转的角度
输入参数：两个编码器的数据
输出参数：无
************************************************/
void motionModel(long leftCnt,long rightCnt)
{
    float instAngle; //采样间隔时间所走的角度
    float instDistance;//采样间隔时间所走的距离

    instAngle = CARMELA_C * (rightCnt - leftCnt)*1.0/(CARMELA_P*CARMELA_B);
    instAngle = instAngle * 180.0 / PI;

    instDistance = CARMELA_C*(rightCnt + leftCnt)*1.0/(CARMELA_P*2.0);

    if(instAngle > 0.2) //认为左转 留有余量
    {
        flagMotionLeft = 1;
        flagMotionRight = 0;
        flagMotionForward = 0;
    }
    else if((instAngle <= 0.2)&&(instAngle >= -0.2))
        //认为前进 留有余量
        {
            flagMotionLeft = 0;
            flagMotionRight = 0;
            flagMotionForward = 1;
        }
        else    //认为右转 留有余量
        {
            flagMotionLeft = 0;
            flagMotionRight = 1;
            flagMotionForward = 0;
        }
}



/********leftfrontEncoder()******************
函数功能：利用函数中断，读取左前轮编码器数据
输入参数：无
输出参数：无
********************************************/
void leftfrontEncoder()
{
  static unsigned long leftfrontTime = 0;

  if(micros()- leftfrontTime>ENCODING_SLOT ) //滤波
  {
    if(0 == digitalRead(LEFT_FRONT_B))
    //根据输出波形可知，A下降沿，B低电平则正转
    {
        leftfrontEncoderCnt ++;
    }
    else
    {
        leftfrontEncoderCnt --;
    }
    leftfrontTime = micros();
  }
}
/********leftrearEncoder()******************
函数功能：利用函数中断，读取左后轮编码器数据
输入参数：无
输出参数：无
********************************************/
void leftrearEncoder()
{
  static unsigned long leftrearTime = 0;

  if(micros()- leftrearTime>ENCODING_SLOT )
  {
    if(0 == digitalRead(LEFT_REAR_B))
    //根据输出波形可知，A下降沿，B低电平则正转
    {
        leftrearEncoderCnt ++;
    }
    else
    {
        leftrearEncoderCnt --;
    }
    leftrearTime = micros();
  }
}
/********rightfrontEncoder()******************
函数功能：利用函数中断，读取右前轮编码器数据
输入参数：无
输出参数：无
********************************************/
void rightfrontEncoder()
{
  static unsigned long rightfrontTime = 0;

  if(micros()- rightfrontTime>ENCODING_SLOT )
  {
    if(0 == digitalRead(RIGHT_FRONT_B))
    //根据输出波形可知，A下降沿，B低电平则正转
    //实测：左右两侧轮子相反。
    {
        rightfrontEncoderCnt --;
    }
    else
    {
        rightfrontEncoderCnt ++;
    }
    rightfrontTime = micros();
  }
}
/********rightrearEncoder()******************
函数功能：利用函数中断，读取右后轮编码器数据
输入参数：无
输出参数：无
********************************************/
void rightrearEncoder()
{
  static unsigned long rightrearTime = 0;

  if(micros()- rightrearTime>ENCODING_SLOT )
  //根据输出波形可知，A下降沿，B低电平则正转
  //实测：左右两侧轮子相反。
  {
    if(0 == digitalRead(RIGHT_REAR_B))
    {
        rightrearEncoderCnt --;
    }
    else
    {
        rightrearEncoderCnt ++;
    }
    rightrearTime = micros();
  }
}








