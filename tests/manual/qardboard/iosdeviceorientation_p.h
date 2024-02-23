// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <CoreMotion/CoreMotion.h>


@interface iOSDeviceOrientationP : NSObject <CLLocationManagerDelegate>
@property (readonly) BOOL isActive;

+ (iOSDeviceOrientationP*)instance;
- (void)setHandler:(iOSDeviceOrientation*) handler;
- (void)start;
- (void)stop;
@end
