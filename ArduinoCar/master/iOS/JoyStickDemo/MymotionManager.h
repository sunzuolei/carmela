//
//  MymotionManager.h
//  Myboatcontroller
//
//  Created by marco_mao on 15/4/27.
//  Copyright (c) 2015年 Mao Siyi. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CoreMotion/CoreMotion.h"

@interface MymotionManager : NSObject

@property (strong,nonatomic) CMMotionManager *motionManager;
@property (strong,nonatomic) NSTimer *updateTimer;
@property (nonatomic) BOOL joystickflag;

+ (id) getMymotionManager;

@end
