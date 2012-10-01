//
//  NSFont+DebugConsole.m
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import "NSFont+DebugConsole.h"

@implementation NSFont (DebugConsole)

+ (NSFont *)debugConsoleInputFont {
    NSFontManager *fm = [NSFontManager sharedFontManager];
    return [fm convertFont:[NSFont userFixedPitchFontOfSize:12]
               toHaveTrait:NSBoldFontMask];
}

+ (NSFont *)debugConsoleOutputFont {
    return [NSFont userFixedPitchFontOfSize:12];
}

@end
