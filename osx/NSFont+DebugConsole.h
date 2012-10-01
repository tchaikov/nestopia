//
//  NSFont+DebugConsole.h
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import <Cocoa/Cocoa.h>

@interface NSFont (DebugConsole)
+ (NSFont *)debugConsoleOutputFont;
+ (NSFont *)debugConsoleInputFont;
@end
