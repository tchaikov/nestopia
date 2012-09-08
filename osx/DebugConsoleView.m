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
- (void)printStoppedByBreakpoint:(Breakpoint*)breakpoint
{
    /// TODO
}

@end
