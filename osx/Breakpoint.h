//
//  Breakpoint.h
//  OpenNestopia
//
//  Created by Kefu Chai on 16/09/12.
//
//

#import <Foundation/Foundation.h>

@class Condition;

@interface Breakpoint : NSObject

@property (nonatomic, copy) NSString *desc;
@property (nonatomic, assign) BOOL enabled;
@property (nonatomic, assign) NSUInteger pc;
@property (nonatomic, strong) Condition *condition;
@end

@interface Condition : NSObject

@property (nonatomic, assign) NSUInteger addr;
@property (nonatomic, readonly) enum AccessMode;

@end