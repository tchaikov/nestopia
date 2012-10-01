//
//  Decoded.m
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import "Decoded.h"

@implementation Decoded

- (NSString *)addressString {
    return [NSString stringWithFormat:@"%04lX", self.address];
}

@end