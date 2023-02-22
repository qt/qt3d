// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#import "ViewController.h"

@interface ViewController () <WCSessionDelegate>

@end

@implementation ViewController

- (void)session:(WCSession *)session activationDidCompleteWithState:(WCSessionActivationState)activationState error:(NSError *)error {

}

- (void)sessionDidBecomeInactive:(WCSession *)session {

}

- (void)sessionDidDeactivate:(WCSession *)session {

}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.

    if ([WCSession isSupported]) {
        WCSession *session = [WCSession defaultSession];
        session.delegate = self;
        [session activateSession];
    }
}

- (IBAction)applyButtonClicked:(id)sender {
    if ([[WCSession defaultSession] isReachable]) {
        NSDictionary *dictionary = @{
                                     @"host" : [self.hostNameTextField text],
                                     @"port" : [self.portTextField text]
                                     };

        [[WCSession defaultSession] sendMessage:dictionary
                                   replyHandler:^(NSDictionary *replyHandler) {}
                                   errorHandler:^(NSError *error) {}];
    }
}

@end
