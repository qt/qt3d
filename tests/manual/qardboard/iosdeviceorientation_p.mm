// Copyright (C) 2021 The Qt Company Ltd.
// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include "iosdeviceorientation.h"
#import "iosdeviceorientation_p.h"
#include <QtCore/qmath.h>

@interface QIOSMotionManager : NSObject {
}

+ (CMMotionManager *)sharedManager;
@end

@interface iOSDeviceOrientationP()
{
    BOOL active;
    iOSDeviceOrientation* handler;
}

@property (strong) CMMotionManager *motionManager;
@end



@implementation iOSDeviceOrientationP

#define kUpdateFrequency 20.0


+ (iOSDeviceOrientationP*)instance
{
    static iOSDeviceOrientationP* _myInstance = 0;
    if (0 == _myInstance)
        _myInstance = [[iOSDeviceOrientationP alloc] init];
    return _myInstance;
}

- (id)init
{
    self = [super init];
    if (self) {
        self.motionManager = [QIOSMotionManager sharedManager]; // [[CMMotionManager alloc] init];
        self.motionManager.deviceMotionUpdateInterval = 1. / kUpdateFrequency;

        active = FALSE;
    }
    return self;
}

- (void)setHandler:(iOSDeviceOrientation*)h
{
    handler = h;
}

- (BOOL)isActive
{
    return active;
}

- (void)start
{
    // Motion updates
    [self.motionManager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXArbitraryCorrectedZVertical
                                                            toQueue:[NSOperationQueue currentQueue]
                                                        withHandler: ^(CMDeviceMotion *motion, NSError *error) {
        //CMAttitude *attitude = motion.attitude;
        //NSLog(@"rotation rate = [%f, %f, %f]", attitude.pitch, attitude.roll, attitude.yaw);
        if (error)
            NSLog(@"%@", [error description]);
        else
            [self performSelectorOnMainThread:@selector(handleDeviceMotion:) withObject:motion waitUntilDone:YES];
    }];
    active = TRUE;
}

- (void)stop
{
    [_motionManager stopDeviceMotionUpdates];
    active = FALSE;
}

- (void)handleDeviceMotion:(CMDeviceMotion*)motion
{
    if (!active)
        return;

    if (motion.magneticField.accuracy == CMMagneticFieldCalibrationAccuracyUncalibrated)
        return;

    // X: A pitch is a rotation around a lateral axis that passes through the device from side to side.
    // Y: A roll is a rotation around a longitudinal axis that passes through the device from its top to bottom.
    // Z: A yaw is a rotation around an axis that runs vertically through the device. It is perpendicular to the
    //    body of the device, with its origin at the center of gravity and directed toward the bottom of the device.

    CMAttitude *attitude = motion.attitude;
    handler->setRoll(90 - qRadiansToDegrees(attitude.roll));
    handler->setPitch(qRadiansToDegrees(attitude.pitch));
    handler->setYaw(qRadiansToDegrees(attitude.yaw));
}

@end
