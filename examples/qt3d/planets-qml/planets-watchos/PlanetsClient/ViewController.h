// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#import <UIKit/UIKit.h>
#import <WatchConnectivity/WatchConnectivity.h>

@interface ViewController : UIViewController
@property (weak, nonatomic) IBOutlet UITextField *hostNameTextField;
@property (weak, nonatomic) IBOutlet UITextField *portTextField;
@property (weak, nonatomic) IBOutlet UIButton *applyButton;

@end

