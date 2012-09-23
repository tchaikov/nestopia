//
//  DebugConsoleView.m
//  OpenNestopia
//
//  Created by Kefu Chai on 17/09/12.
//
//

#import "DebugConsoleView.h"
#import "Breakpoint.h"

@implementation DebugConsoleView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Drawing code here.
}

#pragma mark -
#pragma mark Debug
- (void)printPrompt
{
    [self insertText:@"(ndb) "];
}

- (void)printStoppedByBreakpoint:(Breakpoint*)breakpoint at:(NSUInteger)pc
{
    // see for colored string
    [self insertText:[NSString stringWithFormat:@"Breakpoint %ld, at %#04lx",
                      breakpoint.index, pc]];
}

- (void)print:(NSString *)msg
{
    [self insertText:msg];
}

@end
