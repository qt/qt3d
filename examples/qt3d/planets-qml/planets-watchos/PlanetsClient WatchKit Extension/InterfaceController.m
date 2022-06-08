// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#import "InterfaceController.h"

@interface InterfaceController() <WCSessionDelegate>

@end

@implementation InterfaceController

- (id)init {
    if ((self = [super init])) {
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSString *host = [defaults stringForKey:@"host"];
        int port = [defaults integerForKey:@"port"];

        if (host == nil) {
            self.host = @"127.0.0.1";
        } else {
            self.host = host;
        }

        if (port == 0) {
            self.port = [NSNumber numberWithInt:8080];
        } else {
            self.port = [NSNumber numberWithInt:port];
        }

        self.planets = @[@"Sun",
                         @"Mercury",
                         @"Venus",
                         @"Earth",
                         @"Mars",
                         @"Jupiter",
                         @"Saturn",
                         @"Uranus",
                         @"Neptune",
                         @"Solar System"];

        self.minimumValue = 1.0f;
        self.maximumValue = 10.0f;
    }

    return self;
}

- (void)awakeWithContext:(id)context {
    [super awakeWithContext:context];

    int planetsCount = [self.planets count];

    NSMutableArray *pickerItems = [[NSMutableArray alloc] init];

    for (int i = 0; i < planetsCount; i++) {
        WKPickerItem *item = [WKPickerItem alloc];
        item.title = self.planets[i];
        [pickerItems addObject:item];
    }

    [self.planetPicker setItems:pickerItems];

    [self.hostLabel setText:[NSString stringWithFormat:@"%@:%@", self.host, [NSString stringWithFormat:@"%d", [self.port intValue]]]];
}

- (void)willActivate {
    [super willActivate];

    if ([WCSession isSupported]) {
        WCSession *session = [WCSession defaultSession];
        session.delegate = self;
        [session activateSession];
    }
}

- (IBAction)selectedPlanetChanged:(NSInteger)value {
    NSString *command = [@[@"selectPlanet", self.planets[value]] componentsJoinedByString:@"/"];

    [self sendCommand:command];
}

- (IBAction)rotationSpeedChanged:(float)value {
    NSString* formattedValue = [NSString stringWithFormat:@"%.02f", (value - self.minimumValue) /
                                (self.maximumValue - self.minimumValue)];
    NSString *command = [@[@"setRotationSpeed", formattedValue] componentsJoinedByString:@"/"];

    [self sendCommand:command];
}

- (IBAction)viewingDistanceChanged:(float)value {
    NSString* formattedValue = [NSString stringWithFormat:@"%.02f", (value - self.minimumValue) /
                                (self.maximumValue - self.minimumValue)];
    NSString *command = [@[@"setViewingDistance", formattedValue] componentsJoinedByString:@"/"];

    [self sendCommand:command];
}

- (IBAction)planetSizeChanged:(float)value {
    NSString* formattedValue = [NSString stringWithFormat:@"%.02f", (value - self.minimumValue) /
                                (self.maximumValue - self.minimumValue)];
    NSString *command = [@[@"setPlanetSize", formattedValue] componentsJoinedByString:@"/"];

    [self sendCommand:command];
}


- (void)sendCommand:(NSString *)command {
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];

    NSURLComponents *urlComponents = [[NSURLComponents alloc] init];
    urlComponents.scheme = @"http";
    urlComponents.host = self.host;
    urlComponents.port = self.port;
    urlComponents.path = [NSString stringWithFormat:@"/%@", command];

    [request setURL:urlComponents.URL];
    [request setHTTPMethod:@"GET"];

    NSURLSession *session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]];
    [[session dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response,
                                                              NSError *error) {}] resume];
}

- (void)session:(nonnull WCSession *)session didReceiveMessage:(nonnull NSDictionary *)message replyHandler:(nonnull void (^)(NSDictionary<NSString *,id> *))replyHandler {
    NSString *host = [message objectForKey:@"host"];
    NSString *port = [message objectForKey:@"port"];

    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    numberFormatter.numberStyle = NSNumberFormatterNoStyle;

    self.host = host;
    self.port = [numberFormatter numberFromString:port];

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:host forKey:@"host"];
    [defaults setInteger:[self.port integerValue] forKey:@"port"];
    [defaults synchronize];

    [self.hostLabel setText:[NSString stringWithFormat:@"%@:%@", host, port]];
}

- (void)session:(WCSession *)session activationDidCompleteWithState:(WCSessionActivationState)activationState error:(NSError *)error {

}

@end
