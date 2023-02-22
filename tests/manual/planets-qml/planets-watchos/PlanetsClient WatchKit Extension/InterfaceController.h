// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#import <WatchKit/WatchKit.h>
#import <Foundation/Foundation.h>
#import <WatchConnectivity/WatchConnectivity.h>

@interface InterfaceController : WKInterfaceController

@property (nonatomic, copy) NSString *host;
@property (nonatomic, copy) NSNumber *port;
@property (nonatomic, copy) NSArray *planets;
@property (nonatomic, assign) float minimumValue;
@property (nonatomic, assign) float maximumValue;
@property (unsafe_unretained, nonatomic) IBOutlet WKInterfaceLabel *hostLabel;
@property (unsafe_unretained, nonatomic) IBOutlet WKInterfacePicker *planetPicker;
@property (unsafe_unretained, nonatomic) IBOutlet WKInterfaceSlider *rotationSpeedSlider;
@property (unsafe_unretained, nonatomic) IBOutlet WKInterfaceSlider *viewingDistanceSlider;
@property (unsafe_unretained, nonatomic) IBOutlet WKInterfaceSlider *planetSizeSlider;

@end
