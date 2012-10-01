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
#import "WatchTableController.h"

@interface DebuggerWindowController ()

@end

@implementation DebuggerWindowController

- (void)setGameCore:(NESGameCore *)gameCore
{
    _gameCore = gameCore;
    self.debugger = [[DebuggerBridge alloc] initWithEmu:gameCore.nesEmu];
    _disassembledController.debugger = self.debugger;
    _watchController.debugger = self.debugger;
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
    _disassembledController =
        [[DisassembledTableController alloc] initWithNibName:@"DisassembledTableController" bundle:nil];
    [_disassembledView addSubview:_disassembledController.view];
    _disassembledController.view.frame = _disassembledView.bounds;

    _watchController =
        [[WatchTableController alloc] initWithNibName:@"WatchTableController" bundle:nil];
    [_watchView addSubview:_watchController.view];
    _watchController.view.frame = _watchView.bounds;
}

- (void)showWindow:(id)sender {
    [super showWindow:sender];
    if (self.gameCore.pauseEmulation) {
        // already paused
        [self pausedAtPc:self.gameCore.pc withPrompt:YES];
    }
}

#pragma mark -
- (void)emulatorPaused:(NSNotification *)note {
    NESGameCore *gameCore = [note object];
    [self pausedAtPc:gameCore.pc withPrompt:YES];
}

#pragma mark -
#pragma mark DebuggerDelegate

- (void)willStepToAddress:(NSUInteger)pc
{
    [self pausedAtPc:pc withPrompt:YES];
}

- (void)breakpoint:(NSUInteger)index triggeredAt:(NSUInteger)pc {
    [self pausedAtPc:pc withPrompt:NO];
    Breakpoint * breakpoint = [self.debugger breakpointAtIndex:index];
    NSAssert(breakpoint, @"breakpoint #%ld not found", index);
    [self printStoppedByBreakpoint:breakpoint at:pc];
}

- (void)printConsole:(NSString *)msg {
    [self.consoleView print:msg];
}

#pragma mark -
#pragma mark private
- (void)pausedAtPc:(NSUInteger)pc withPrompt:(BOOL)prompt
{
    [_disassembledController updateWithPc:pc];
    [_watchController update];
    if (prompt) {
        [self printPrompt];
    }
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
