//
//  VisualStickView.h
//  Myboatcontroller
//
//  Created by marco_mao on 15/4/27.
//  Copyright (c) 2015年 Mao Siyi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CoreMotion/CoreMotion.h"
#import "MymotionManager.h"

@interface JoyStickView : UIView

{
    IBOutlet UIImageView *stickViewBase;
    IBOutlet UIImageView *stickView;
    
    CGPoint mCenter;
    MymotionManager *mymotionManager;
}

@end
