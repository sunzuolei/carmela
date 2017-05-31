//
//  VisualStickView.m
//  Myboatcontroller
//
//  Created by marco_mao on 15/4/27.
//  Copyright (c) 2015年 Mao Siyi. All rights reserved.
//

#import "JoyStickView.h"

#define STICK_CENTER_TARGET_POS_LEN 20.0f

@implementation JoyStickView

- (void) initStick
{
    //摇杆图片大小都是128*128的，所以摇杆中心位置mCenter在初始化函数中设置为(64,64)
    mCenter.x = 64;
    mCenter.y = 64;
    mymotionManager = [MymotionManager getMymotionManager]; //获取对象mymotionManager
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self initStick];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self)
	{
        [self initStick];
    }
    return self;
}

- (void)notifyDir:(CGPoint)dir
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    //定义userInfo字典用来存放摇杆变化数据
    NSValue *vdir = [NSValue valueWithCGPoint:dir];
    NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:
                              vdir, @"dir", nil];
    
    //定义通知发送userInfo字典
    NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
    [notificationCenter postNotificationName:@"StickChanged" object:nil userInfo:userInfo];
    
    [pool release];
}

//改变摇杆位置方法
- (void)stickMoveTo:(CGPoint)deltaToCenter
{
    CGRect fr = stickView.frame;
    fr.origin.x = deltaToCenter.x;
    fr.origin.y = deltaToCenter.y;
    stickView.frame = fr;
}

//计算并设置摇杆位置
- (void)touchEvent:(NSSet *)touches
{
    //限定单点触控
    if([touches count] != 1)
        return ;
    
    UITouch *touch = [touches anyObject];
    UIView *view = [touch view];
    if(view != self)
        return ;
    
    CGPoint touchPoint = [touch locationInView:view];
    CGPoint dtarget, dir;    //dir为摇杆中心到触摸位置的方向矢量
    dir.x = touchPoint.x - mCenter.x;
    dir.y = touchPoint.y - mCenter.y;
    double len = sqrt(dir.x * dir.x + dir.y * dir.y);
    double len_inv = (1.0 / len);
    dir.x *= len_inv;
    dir.y *= len_inv;   //对dir做归一化
    dtarget.x = dir.x * STICK_CENTER_TARGET_POS_LEN;
    dtarget.y = dir.y * STICK_CENTER_TARGET_POS_LEN;    //dtarget为摇杆现在的位置

    [self stickMoveTo:dtarget]; //调用stickMoveTo方法将摇杆移动到dtarget
    [self notifyDir:dir];   //将dir数据通过通知发送
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (mymotionManager.joystickflag) { //只有当摇杆生效标志joystickflag打开时，摇杆才有控制权
        stickView.image = [UIImage imageNamed:@"stick_hold.png"];   //触摸摇杆时，更换摇杆图片
        [self touchEvent:touches];
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (mymotionManager.joystickflag) { //只有当摇杆生效标志joystickflag打开时，摇杆才有控制权
        [self touchEvent:touches];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    stickView.image = [UIImage imageNamed:@"stick_normal.png"]; //停止触摸摇杆时，更换回摇杆静止图片
    
    //使摇杆归位
    CGPoint dtarget, dir;
    dir.x = dtarget.x = 0.0;
    dir.y = dtarget.y = 0.0;
    [self stickMoveTo:dtarget];
    [self notifyDir:dir];
}

@end
