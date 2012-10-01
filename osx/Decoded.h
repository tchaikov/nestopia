//
//  Decoded.h
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import <Foundation/Foundation.h>

@interface Decoded : NSObject
@property (copy) NSString *description;
@property (copy) NSString *repr;
@property (assign) NSUInteger address;
@property (readonly) NSString *addressString;
@end
