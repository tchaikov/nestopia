//
//  Breakpoint.m
//  OpenNestopia
//
//  Created by Kefu Chai on 16/09/12.
//
//

#import "Breakpoint.h"

@implementation Breakpoint


- (id)initWithAddress:(NSUInteger)address
               access:(AccessMode)access
              enabled:(BOOL)enabled {
    if ((self = [super init]) != nil) {
        self.address = address;
        self.access = access;
        self.enabled = enabled;
    }
    return self;
}

- (NSString *)description
{
    NSString *access_str;
    switch (self.access) {
        case AccessRead:
            access_str = @"read";
            break;
        case AccessWrite:
            access_str = @"write";
            break;
        case AccessExec:
            access_str = @"exec";
            break;
        case AccessRW:
            access_str = @"rw";
            break;
        default:
            access_str = @"unknown";
    }
    return [NSString stringWithFormat:@"break at: %lul:%@, %@",
            self.address, access_str, self.enabled ? @"enabled" : @"disabled"];
}

@end
