//
//  DebugConsoleView.h
//  OpenNestopia
//
//  Created by Kefu Chai on 17/09/12.
//
//

#import <Cocoa/Cocoa.h>

@class Breakpoint;

@interface DebugConsoleView : NSTextView

- (void)printStoppedByBreakpoint:(Breakpoint *)breakpoint at:(NSUInteger)pc;
- (void)print:(NSString *)msg;

@end
