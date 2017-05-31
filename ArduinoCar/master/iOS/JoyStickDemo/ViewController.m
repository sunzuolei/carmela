//
//  ViewController.m
//  Myboatcontroller
//
//  Created by marco_mao on 15/4/27.
//  Copyright (c) 2015年 Mao Siyi. All rights reserved.
//

#import "ViewController.h"

@implementation ViewController

@synthesize userinfoshow;

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background.png"]];    //设置UI界面背景图片
    
    //观察虚拟遥杆位置变化通知
    NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
    [notificationCenter addObserver: self
                           selector: @selector (onStickChanged:)
                               name: @"StickChanged"
                             object: nil];
    
    //获取对象mymotionManager，并将虚拟摇杆生效标志joystickflag打开
    mymotionManager = [MymotionManager getMymotionManager];
    mymotionManager.joystickflag = YES;
    
    //与智能船建立TCP连接，在控制台以及UI上打印连接状态
    asyncSocket = [[AsyncSocket alloc] initWithDelegate:self];
    NSError *err = nil;
    if(![asyncSocket connectToHost:@"192.168.11.254" onPort:8080 error:&err])
    {
        NSLog(@"Error: %@", err);
    } else {
        NSLog(@"connect!");
        self.userinfoshow.text = @"The car has been connected!";
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

//接受到虚拟摇杆位置变化通知之后的执行方法入口
- (void)onStickChanged:(id)notification
{
    //创建dict字典存放虚拟摇杆位置变化数据
    NSDictionary *dict = [notification userInfo];
    NSValue *vdir = [dict valueForKey:@"dir"];
    CGPoint dir = [vdir CGPointValue];
    NSMutableData *data = [[NSMutableData alloc] init];
    NSKeyedArchiver *archiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:data];
    [archiver encodeObject:vdir forKey:@"dir"];
    [archiver finishEncoding];
    
    //定义智能船姿态控制档位标志
    int moveStatus;
    
//    //设置智能船左右姿态控制档位范围
//    if (dir.x > -1.0 && dir.x <= -0.7) {
//        lmrflag = -2;   //左满舵
//    }
//    if (dir.x > -0.7 && dir.x <= -0.4) {
//        lmrflag = -1;   //左舵
//    }
//    if (dir.x >= 0.4 && dir.x < 0.7) {
//        lmrflag = 1;    //右舵
//    }
//    if (dir.x >= 0.7 && dir.x < 1.0) {
//        lmrflag = 2;    //右满舵
//    }
//    if ((dir.x > -0.4 && dir.x < 0.4) || dir.x <= -1.0 || dir.x >= 1.0) {
//        lmrflag = 0;    //正舵
//    }
//    
//    //设置智能船前后姿态控制档位范围
//    if (dir.y > -1.0 && dir.y <= -0.5) {
//        dnrflag = 2;    //全速前进
//    }
//    if (dir.y > -0.5 && dir.y < 0) {
//        dnrflag = 1;    //慢速前进
//    }
//    if (dir.y > 0 && dir.y < 1.0) {
//        dnrflag = -1;   //慢速倒退
//    }
//    if (dir.y == 0 || dir.y <= -1.0 || dir.y >= 1.0) {
//        dnrflag = 0;    //停
//    }
    float x = dir.x;
    float y = - dir.y;
    float distance = powf(x, 2) + powf(y, 2);
    float distanceMin = powf(0.4, 2);
    float distanceMax = powf(1, 2);
    
    if ( (fabs(y) > fabs(x)) && (y > 0) && ( (distance > distanceMin) || (distance < distanceMax) ) ) {
        moveStatus = 1;
    }
    if ( (fabs(y) > fabs(x)) && (y < 0) && ( (distance > distanceMin) || (distance < distanceMax) ) ) {
        moveStatus = 2;
    }
    if ( (fabs(y) < fabs(x)) && (x > 0) && ( (distance > distanceMin) || (distance < distanceMax) ) ) {
        moveStatus = 3;
    }
    if ( (fabs(y) < fabs(x)) && (x < 0) && ( (distance > distanceMin) || (distance < distanceMax) ) ) {
        moveStatus = 4;
    }
    if (distance < distanceMin) {
        moveStatus = 0;
    }
    //依据智能船姿态控制档位标志判断智能船接受到的指令是否变化，若变化，则发送新指令到智能船
    if (moveStatus != statusCheck) {
        switch (moveStatus) {
            case 1:
                [self moveForward];
                break;
                
            case 2:
                [self moveBackward];
                break;
            
            case 3:
                [self moveRight];
                break;
            
            case 4:
                [self moveLeft];
                break;
            
            case 0:
                [self moveStop];
                break;
                
            default:
                break;
        }
    }
    
    //更新智能船姿态控制档位标志
    statusCheck = moveStatus;
}

//重力感应控制算法
- (void)updateDisplay {
    if (mymotionManager.motionManager.accelerometerAvailable) {
        CMAccelerometerData *accelerometerData = mymotionManager.motionManager.accelerometerData;
        
        if (accelerometerData.acceleration.x <= 1 && accelerometerData.acceleration.x >= -1 && accelerometerData.acceleration.y <= 1 && accelerometerData.acceleration.y >= -1) {    //限定加速度计x，y值在－1到1之间才进行对智能船的控制
            
            //定义智能船姿态控制档位标志
            int moveStatus;
            
            //设置智能船左右姿态控制档位范围
//            if (accelerometerData.acceleration.y>-1 && accelerometerData.acceleration.y<=-0.7) {
//                lmrflag = 2;    //右满舵
//            }
//            if (accelerometerData.acceleration.y>-0.7 && accelerometerData.acceleration.y<=-0.4) {
//                lmrflag = 1;    //右舵
//            }
//            if (accelerometerData.acceleration.y>=0.4 && accelerometerData.acceleration.y<0.7) {
//                lmrflag = -1;   //左舵
//            }
//            if (accelerometerData.acceleration.y>=0.7 && accelerometerData.acceleration.y<1) {
//                lmrflag = -2;   //左满舵
//            }
//            if ((accelerometerData.acceleration.y>-0.4 && accelerometerData.acceleration.y<0.4) || accelerometerData.acceleration.y<=-1 || accelerometerData.acceleration.y>=1) {
//                lmrflag = 0;    //正舵
//            }
//            
//            //设置智能船前后姿态控制档位范围
//            if (accelerometerData.acceleration.x>=0.5 && accelerometerData.acceleration.x<1) {
//                dnrflag = 2;    //全速前进
//            }
//            if (accelerometerData.acceleration.x>=0.4 && accelerometerData.acceleration.x<0.5) {
//                dnrflag = 1;    //慢速前进
//            }
//            if (accelerometerData.acceleration.x<=-0.4 && accelerometerData.acceleration.x>-1) {
//                dnrflag = -1;   //慢速倒退
//            }
//            if ((accelerometerData.acceleration.x>-0.4 && accelerometerData.acceleration.x<0.4) || accelerometerData.acceleration.x<=-1 || accelerometerData.acceleration.x>=1) {
//                dnrflag = 0;    //停
//            }
            
            float x = - accelerometerData.acceleration.y;
            float y = accelerometerData.acceleration.x;
            float distance = powf(x, 2) + powf(y, 2);
            float distanceMin = powf(0.4, 2);
            float distanceMax = powf(1, 2);
            
            if ( (fabs(y) > fabs(x)) && (y > 0) && ( (distance > distanceMin) || (distance < distanceMax) ) ) {
                moveStatus = 1;
            }
            if ( (fabs(y) > fabs(x)) && (y < 0) && ( (distance > distanceMin) || (distance < distanceMax) ) ) {
                moveStatus = 2;
            }
            if ( (fabs(y) < fabs(x)) && (x > 0) && ( (distance > distanceMin) || (distance < distanceMax) ) ) {
                moveStatus = 3;
            }
            if ( (fabs(y) < fabs(x)) && (x < 0) && ( (distance > distanceMin) || (distance < distanceMax) ) ) {
                moveStatus = 4;
            }
            if (distance < distanceMin) {
                moveStatus = 0;
            }
            //依据智能船姿态控制档位标志判断智能船接受到的指令是否变化，若变化，则发送新指令到智能船
            if (moveStatus != statusCheck) {
                switch (moveStatus) {
                    case 1:
                        [self moveForward];
                        break;
                        
                    case 2:
                        [self moveBackward];
                        break;
                        
                    case 3:
                        [self moveRight];
                        break;
                        
                    case 4:
                        [self moveLeft];
                        break;
                        
                    case 0:
                        [self moveStop];
                        break;
                        
                    default:
                        break;
                }
            }
            
            //更新智能船姿态控制档位标志
            statusCheck = moveStatus;
        }
    }
}

//虚拟摇杆与重力感应控制权选择开关
- (IBAction)controlchoose:(UISegmentedControl *)sender {
    NSInteger Index = sender.selectedSegmentIndex;
    switch (Index) {
        //重力感应控制
        case 1:
            mymotionManager.joystickflag = NO;  //关闭虚拟摇杆生效标志joystickflag
            [mymotionManager.motionManager startAccelerometerUpdates];  //启动加速度计
            mymotionManager.updateTimer = [NSTimer
                                scheduledTimerWithTimeInterval:1.0/10.0 //设置加速度计数据更新速率
                                target:self
                                selector:@selector(updateDisplay)   //执行重力感应控制算法
                                userInfo:nil
                                repeats:YES];
            break;
        
        //虚拟摇杆控制
        case 0:
            mymotionManager.joystickflag = YES; //打开虚拟摇杆生效标志joystickflag
            [mymotionManager.motionManager stopAccelerometerUpdates];   //关闭加速度计
            [mymotionManager.updateTimer invalidate];   //关闭加速度计数据更新计时器
            mymotionManager.updateTimer = nil;
            break;
            
        default:
            break;
    }
}

//重新建立TCP连接按钮
- (IBAction)reconnect:(UIButton *)sender {
    asyncSocket = [[AsyncSocket alloc] initWithDelegate:self];
    NSError *err = nil;
    [asyncSocket connectToHost:@"192.168.11.254" onPort:8080 error:&err];
}

- (void) moveStop {
    NSData* stopOrder= [@"0" dataUsingEncoding: NSUTF8StringEncoding];
    [asyncSocket writeData:stopOrder withTimeout:-1 tag:1];
    self.userinfoshow.text = [NSString stringWithFormat:
                                  @"      Car Status:\n----------------------\n"
                                  "-Stop-"];
}

- (void) moveForward {
    NSData* forwardOrder= [@"1" dataUsingEncoding: NSUTF8StringEncoding];
    [asyncSocket writeData:forwardOrder withTimeout:-1 tag:1];
    self.userinfoshow.text = [NSString stringWithFormat:
                                  @"      Car Status:\n----------------------\n"
                                  "-Move Forward-"];
}

- (void) moveBackward {
    NSData* backwardOrder= [@"2" dataUsingEncoding: NSUTF8StringEncoding];
    [asyncSocket writeData:backwardOrder withTimeout:-1 tag:1];
    self.userinfoshow.text = [NSString stringWithFormat:
                                  @"      Car Status:\n----------------------\n"
                                  "-Move Backward-"];
}

- (void) moveRight {
    NSData* rightOrder= [@"3" dataUsingEncoding: NSUTF8StringEncoding];
    [asyncSocket writeData:rightOrder withTimeout:-1 tag:1];
    self.userinfoshow.text = [NSString stringWithFormat:
                                  @"      Car Status:\n----------------------\n"
                                  "-Move Right-"];
}

- (void) moveLeft {
    NSData* leftOrder= [@"4" dataUsingEncoding: NSUTF8StringEncoding];
    [asyncSocket writeData:leftOrder withTimeout:-1 tag:1];
    self.userinfoshow.text = [NSString stringWithFormat:
                                  @"      Car Status:\n----------------------\n"
                                  "-Move Left-"];    //在UI界面的lable中打印最新控制指令信息
}

@end
