//
//  DebuggerDelegate.h
//  OpenNestopia
//
//  Created by Kefu Chai on 16/09/12.
//
//

#import <Foundation/Foundation.h>

@protocol DebuggerDelegate <NSObject>

@required
- (void) executeDoneAt:(NSUInteger)pc;
- (void) breakpoint:(NSUInteger)breakpoint triggeredAt:(NSUInteger)pc;

@end
