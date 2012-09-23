//
//  DebuggerController.m
//  OpenNestopia
//
//  Created by Kefu Chai on 04/09/12.
//
//

#import "DebuggerWindowController.h"
#import "Breakpoint.h"
#import "DebuggerBridge.h"
#import "DebugConsoleView.h"

@interface DebuggerWindowController ()

@end

@implementation DebuggerWindowController

- (void)setEmu:(void *)emu
{
    self.debugger = [[DebuggerBridge alloc] initWithEmu:emu];
}

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

#pragma mark -
#pragma mark DebuggerDelegate
- (void)executeDoneAt:(NSUInteger)pc
{
    [self updateDisassemblyWindowStartsAt:pc];
    [self updateWatchWindow];
}

- (void)breakpoint:(NSUInteger)breakpointIndex triggeredAt:(NSUInteger)pc
{
    Breakpoint * breakpoint = [self breakpointAtIndex:breakpointIndex];
    NSAssert(breakpoint, @"breakpoint #%ld not found", breakpointIndex);
    [self.consoleView printStoppedByBreakpoint:breakpoint at:pc];
    [self updateDisassemblyWindowStartsAt:pc];
    [self updateWatchWindow];
}

- (void)printConsole:(NSString *)msg {
    [self.consoleView print:msg];
}

#pragma mark -
#pragma mark private
- (void)updateWatchWindow
{
    // call debuggerBridge to disassemble the instruction at/after pc
    /// @todo
}

- (void)updateDisassemblyWindowStartsAt:(NSUInteger)pc
{
    // call debuggerBridge to fetch all watched memory address and registers
    // in watch window
    /// @todo
}

- (Breakpoint *)breakpointAtIndex:(NSUInteger)index
{
    // debugger
    return nil;
}

- (void)pauseDebugger
{
    
}
@end
