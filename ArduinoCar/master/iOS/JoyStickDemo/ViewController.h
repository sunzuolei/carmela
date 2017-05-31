//
//  ViewController.h
//  Myboatcontroller
//
//  Created by marco_mao on 15/4/27.
//  Copyright (c) 2015年 Mao Siyi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AsyncSocket.h"
#import "MymotionManager.h"

@interface ViewController : UIViewController{
    MymotionManager *mymotionManager;
    AsyncSocket *asyncSocket;
    
    //定义智能船姿态控制档位检查标志
    int dnrcheck;
    int lmrcheck;
    int statusCheck;
}

@property (retain, nonatomic) IBOutlet UILabel *userinfoshow;
@property (weak, nonatomic) IBOutlet UILabel *Ssid;

- (IBAction)reconnect:(UIButton *)sender;

- (IBAction)controlchoose:(UISegmentedControl *)sender;

@end
