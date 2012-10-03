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

/// called when in step {over, in} mode
- (void)willStepToAddress:(NSUInteger)pc;
/// called in run-until mode
- (void)breakpoint:(NSUInteger)breakpoint triggeredAt:(NSUInteger)pc;
/// convenient method in case debugger wanna say something.
/// if we really want stick to the MVC strictly, this method should not exist at
/// all...
- (void)printConsole:(NSString *)msg,...;

@end
