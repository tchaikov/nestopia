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
#import "NESGameCore.h"
#import "DisassembledTableController.h"

@interface DebuggerWindowController ()

@end

@implementation DebuggerWindowController

- (void)setGameCore:(NESGameCore *)gameCore
{
    _gameCore = gameCore;
    self.debugger = [[DebuggerBridge alloc] initWithEmu:gameCore.nesEmu];
    _disassembledController.debugger = self.debugger;
}


- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
    }
    
    return self;
}

- (void)awakeFromNib {
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(emulatorPaused:)
                                                 name:NESEmulatorDidPauseNotification
                                               object:nil];
    _disassembledController = [[DisassembledTableController alloc] initWithNibName:@"DisassembledTableController" bundle:nil];
    [self.disassembledView addSubview:_disassembledController.view];
    _disassembledController.view.frame = self.disassembledView.bounds;
}

- (void)showWindow:(id)sender {
    [super showWindow:sender];
    if (self.gameCore.pauseEmulation) {
        // already paused
        [self updateAllWindowsWithPc:self.gameCore.pc];
        [self printPrompt];
    }
}

#pragma mark -
- (void)emulatorPaused:(NSNotification *)note {
    NESGameCore *gameCore = [note object];
    [self updateAllWindowsWithPc:gameCore.pc];
    [self printPrompt];
}

#pragma mark -
#pragma mark DebuggerDelegate

- (void)willStepToAddress:(NSUInteger)pc
{
    [self updateAllWindowsWithPc:pc];
    [self printPrompt];
}

- (void)breakpoint:(NSUInteger)breakpointIndex triggeredAt:(NSUInteger)pc {
    Breakpoint * breakpoint = [self breakpointAtIndex:breakpointIndex];
    NSAssert(breakpoint, @"breakpoint #%ld not found", breakpointIndex);
    [self printStoppedByBreakpoint:breakpoint at:pc];
    [self updateAllWindowsWithPc:pc];
}

- (void)printConsole:(NSString *)msg {
    [self.consoleView print:msg];
}

- (void)updateWithCurrentAddress:(NSUInteger)pc {
}

#pragma mark -
#pragma mark private
- (void)updateAllWindowsWithPc:(NSUInteger)pc
{
    [_disassembledController updateDisassemblyWindowWithPc:pc];
    [self updateWatchWindow];
}

- (void)updateWatchWindow
{
    // call debuggerBridge to disassemble the instruction at/after pc
    /// @todo
}

- (Breakpoint *)breakpointAtIndex:(NSUInteger)index {
    // debugger
    return nil;
}

- (void)pauseDebugger {
    
}


#pragma mark -
#pragma mark NSTextView delegate methods

- (void)printPrompt {
    NSString *prompt = @"(ndb) ";
    [self.consoleView insertText:prompt];
    NSRange range = NSMakeRange(committedLength, prompt.length);
    [self.consoleView setFont:[NSFont userFixedPitchFontOfSize:12]
                        range:range];
    [self.consoleView setTextColor:[NSColor colorWithSRGBRed:0.34
                                                       green:0.43
                                                        blue:1
                                                       alpha:1]
                             range:range];
    committedLength = self.consoleView.string.length;
}

- (void)printStoppedByBreakpoint:(Breakpoint*)breakpoint at:(NSUInteger)pc
{
    // see for colored string
    [self.consoleView insertText:[NSString stringWithFormat:@"Breakpoint %ld, at %#04lx",
                      breakpoint.index, pc]];
    committedLength = self.consoleView.string.length;
}

- (void)print:(NSString *)msg
{
    [self insertText:msg];
}

- (BOOL)textView:(NSTextView *)textView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString {
    // Allow changes only for uncommitted text
    return affectedCharRange.location >= committedLength;
}

- (BOOL)textView:(NSTextView *)textView doCommandBySelector:(SEL)commandSelector {
    BOOL retval = NO;

    // When return is entered, record and color the newly committed text
    if (@selector(insertNewline:) == commandSelector) {

        NSUInteger textLength = [[textView string] length];
        if (textLength > committedLength) {
            [textView setSelectedRange:NSMakeRange(textLength, 0)];
            [textView insertText:@"\n"];
            textLength++;
            committedLength = textLength;
        }
        retval = YES;
    }
    return retval;
}

@end
