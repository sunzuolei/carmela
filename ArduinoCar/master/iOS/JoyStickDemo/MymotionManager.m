//
//  MymotionManager.m
//  Myboatcontroller
//
//  Created by marco_mao on 15/4/27.
//  Copyright (c) 2015年 Mao Siyi. All rights reserved.
//

#import "MymotionManager.h"

@implementation MymotionManager

@synthesize motionManager;
@synthesize updateTimer;
@synthesize joystickflag;

//建立motionManager实例给其它类调用
+ (id) getMymotionManager{
    static MymotionManager *sharedmotionManager = nil;
    static dispatch_once_t predicate;
    dispatch_once(&predicate,^{
        sharedmotionManager = [[self alloc] init];
    });
    return sharedmotionManager;
}

//初始化motionManager
-(id) init {
    self = [super init];
    if (self) {
        self.motionManager = [[CMMotionManager alloc] init];
        if (self.motionManager.accelerometerAvailable) {
            self.motionManager.accelerometerUpdateInterval = 1.0/10.0;
        }
    }
    return self;
}

@end
