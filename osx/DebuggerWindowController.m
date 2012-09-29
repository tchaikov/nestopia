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
#import "NESGameCore.h"

@interface DebuggerWindowController ()

@end

@implementation DebuggerWindowController

- (void)setGameCore:(NESGameCore *)gameCore
{
    _gameCore = gameCore;
    self.debugger = [[DebuggerBridge alloc] initWithEmu:gameCore.nesEmu];
}

#define DISASSEMBLY_WINDOW_SIZE 200  // display at most 200 instructions in the window

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        _disassembled = [[NSMutableArray alloc] initWithCapacity:DISASSEMBLY_WINDOW_SIZE];
    }
    
    return self;
}

- (void)awakeFromNib {
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(emulatorPaused:)
                                                 name:NESEmulatorDidPauseNotification
                                               object:nil];
}

- (void)showWindow:(id)sender {
    [super showWindow:sender];
    if (self.gameCore.pauseEmulation) {
        // already paused
        [self updateAllWindowsWithPc:self.gameCore.pc];
    }
}

#pragma mark -
- (void)emulatorPaused:(NSNotification *)note {
    NESGameCore *gameCore = [note object];
    [self updateAllWindowsWithPc:gameCore.pc];
}

#pragma mark -
#pragma mark DebuggerDelegate

- (NSUInteger)indexOfOpcodeHigherThanAddress:(NSUInteger)addr
{
    NSUInteger index = [_disassembled indexOfObjectPassingTest:^(Decoded *obj,
                                                                 NSUInteger index,
                                                                 BOOL *stop) {
        if (obj.address >= addr) {
            *stop = YES;
            return YES;
        } else {
            return NO;
        }
    }];
    return index;
}

- (NSUInteger)indexOfOpcodeAtAddress:(NSUInteger)addr
{
    NSUInteger index = [_disassembled indexOfObjectPassingTest:^(Decoded *obj,
                                                                 NSUInteger index,
                                                                 BOOL *stop) {
        if (obj.address == addr) {
            *stop = YES;
            return YES;
        } else {
            return NO;
        }
    }];
    return index;
}

#define DISASSEMBLY_WINDOW_AHEAD 50  // display at least 50 instructions ahead of pc
#define MAX_INSTRUCTION_LENGTH 3     // it takes at most 3 bytes to store an instruction
- (void)willStepToAddress:(NSUInteger)pc
{
    [self updateAllWindowsWithPc:pc];
}

- (void)breakpoint:(NSUInteger)breakpointIndex triggeredAt:(NSUInteger)pc {
    Breakpoint * breakpoint = [self breakpointAtIndex:breakpointIndex];
    NSAssert(breakpoint, @"breakpoint #%ld not found", breakpointIndex);
    [self.consoleView printStoppedByBreakpoint:breakpoint at:pc];
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
    [self updateDisassemblyWindowWithPc:pc];
    [self updateWatchWindow];
}

- (void)updateWatchWindow
{
    // call debuggerBridge to disassemble the instruction at/after pc
    /// @todo
}

- (void)updateDisassemblyWindowWithPc:(NSUInteger)pc
{
    // fill up the disassembled opcode table view if we are running out of current
    // window
    [self.disassembledView beginUpdates];

    NSIndexSet *indexesAdded = nil;
    NSUInteger expectedLastAddr = pc + MAX_INSTRUCTION_LENGTH * DISASSEMBLY_WINDOW_SIZE;
    if ([self indexOfOpcodeHigherThanAddress:expectedLastAddr] == NSNotFound) {
        // it happens all the time, if we are not in a loop.
        NSUInteger firstAddr = 0, lastAddr = 0;
        if (_disassembled.count > 0) {
            firstAddr = [(Decoded *)_disassembled[0] address];
            lastAddr = [(Decoded *)_disassembled.lastObject address];
        }
        if (pc < firstAddr || pc >= lastAddr) {
            // we are way far from current window, so reset the beginning address
            lastAddr = pc;
            [_disassembled removeAllObjects];
        }
        NSUInteger prevCount = _disassembled.count;
        while (lastAddr < expectedLastAddr) {
            [_disassembled addObject:[self.debugger disassemble:&lastAddr]];
            NSLog(@"lastAddr = %#04lx, expectedLastAddr = %#04lx", lastAddr, expectedLastAddr);
        }
        NSRange range = {prevCount, _disassembled.count - prevCount};
        indexesAdded = [NSIndexSet indexSetWithIndexesInRange:range];
    }

    // if the window is too large, shrink it.
    NSIndexSet *indexesRemoved = nil;
    if (_disassembled.count > DISASSEMBLY_WINDOW_SIZE) {
        NSRange range = {0, _disassembled.count - DISASSEMBLY_WINDOW_SIZE};
        [_disassembled removeObjectsInRange:range];
        indexesRemoved = [NSIndexSet indexSetWithIndexesInRange:range];
    }
    if (indexesAdded) {
        [self.disassembledView insertRowsAtIndexes:indexesAdded
                                     withAnimation:(NSTableViewAnimationEffectFade|
                                                    NSTableViewAnimationSlideUp)];
    }
    if (indexesRemoved) {
        [self.disassembledView removeRowsAtIndexes:indexesRemoved
                                     withAnimation:(NSTableViewAnimationEffectFade|
                                                    NSTableViewAnimationSlideUp)];
    }
    [self.disassembledView endUpdates];

    // highlight current instruction
    NSUInteger currentIndex = [self indexOfOpcodeAtAddress:pc];
    NSAssert(currentIndex != NSNotFound, @"current instruction %ld not in window", pc);
    NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:currentIndex];
    [self.disassembledView scrollRowToVisible:currentIndex];
    [[self.disassembledView animator] selectRowIndexes:indexSet byExtendingSelection:NO];
}

- (Breakpoint *)breakpointAtIndex:(NSUInteger)index {
    // debugger
    return nil;
}

- (void)pauseDebugger {
    
}

#pragma mark -
#pragma mark NSTableView delegate/datasource methods
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return _disassembled.count;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    NSDictionary *dictionary = _disassembled[row];
    // assuming the identifiers of the columns are the same as the propertie names
    // of class Opcode
    return [dictionary valueForKey:[tableColumn identifier]];
}


@end
