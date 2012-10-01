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

- (void)display:(NSUInteger)address;
- (void)set:(uint16_t)address withValue:(uint8_t)value;
- (void)setBreakpoint:(Breakpoint *)bp;
- (void)removeBreakpoint:(NSUInteger)index;
- (void)disableBreakpoint:(NSUInteger)index;
- (void)enableBreakpoint:(NSUInteger)index;
- (void)next;
- (void)stepIn;
- (void)until;
- (void)watch:(NSUInteger)address;
- (void)unwatch:(NSUInteger)index;
- (void)searchBytes:(NSData *)bytes;

@end
