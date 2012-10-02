//
//  CommandRunner.h
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import <Foundation/Foundation.h>

@class Breakpoint;

@protocol CommandRunner <NSObject>

- (void)printVar:(NSUInteger)address;
- (void)set:(uint16_t)address withValue:(uint8_t)value;
- (void)setBreakpoint:(Breakpoint *)bp;
- (void)removeBreakpoint:(NSUInteger)index;
- (void)disableBreakpoint:(NSUInteger)index;
- (void)enableBreakpoint:(NSUInteger)index;
- (void)next;
- (void)stepIn;
- (void)until;
- (void)display:(NSString *)var;
- (void)undisplay:(NSUInteger)index;
- (void)searchBytes:(NSData *)bytes;
- (void)repeatLastCommand;

@end
