#include <Arduino.h>
#include "src/parsePose.h"
#include "src/CarmelaReport.h"
#include "src/parsePackage.h"
#include "src/CarmelaAction.h"

//�����ȡ�������Ĺܽ�
#define LEFT_FRONT_A  0 //pin 2  //interrupt pin
#define LEFT_FRONT_B  5 // General pin
#define LEFT_REAR_A  1  // pin 3
#define LEFT_REAR_B  6
#define RIGHT_FRONT_A  2 //pin 21
#define RIGHT_FRONT_B  7
#define RIGHT_REAR_A  3 // pin 20
#define RIGHT_REAR_B  8
#define ENCODING_SLOT   2  //����������ʱ���Ӳ�

#define BAUD    9600 //�����ʵ�����

//�����жϺ���
void leftfrontEncoder();
void leftrearEncoder();
void rightfrontEncoder();
void rightrearEncoder();


const int LED = 13;

void txetLED()//�����ú�����1S����
{
  static unsigned long timeLED = 0;
  static char  LEDState = 0;

  if(millis() - timeLED > 1000)
  {
    timeLED = millis();

    LEDState = !LEDState;
    digitalWrite(LED,LEDState);
  }
}

void testAction()//�����ú���
{
    long testTime;

    testTime = millis();
    if(testTime > 5000 && testTime < 6000)
    {
      CarmelaForward();
    }
    if(testTime > 6000 && testTime < 7000)
    {
        CarmelaStop();
    }
}

void setup()
{
  Serial3.begin(BAUD); //����3��������ͨ��
  Serial1.begin(BAUD); //����1���ڸ���������巢������

  pinMode(LED,OUTPUT); //LED�ܽ����ڲ���

  pinMode(LEFT_FRONT_A,INPUT); //4��8���������ӿڣ�����Ϊ����
  pinMode(LEFT_FRONT_B,INPUT);
  pinMode(LEFT_REAR_A,INPUT);
  pinMode(LEFT_REAR_B,INPUT);
  pinMode(RIGHT_FRONT_A ,INPUT);
  pinMode(RIGHT_FRONT_B ,INPUT);
  pinMode(RIGHT_REAR_A,INPUT);
  pinMode(RIGHT_REAR_B,INPUT);

 //���ⲿ�жϵķ�ʽ��ȡ4���������Ķ������½��ش����ж�
  attachInterrupt(LEFT_FRONT_A,leftfrontEncoder,FALLING);
  attachInterrupt(LEFT_REAR_A,leftrearEncoder,FALLING);
  attachInterrupt(RIGHT_FRONT_A,rightfrontEncoder,FALLING);
  attachInterrupt(RIGHT_REAR_A,rightrearEncoder,FALLING);

  sampleInterval = 500; //�������ϱ�ʱ����,Ĭ��Ϊ0.5s
  flagCarmelaReplyRAWDATA = true;//Ĭ���ϱ�������ԭʼ����
}

void loop()
{
    startParsePackage();//������λ������������
    report();//�����˶�״̬���ϱ���

    txetLED();
    testAction();
    delay(20);
}






