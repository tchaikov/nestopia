//
//  DisassembledTableController.m
//  OpenNestopia
//
//  Created by Kefu Chai on 30/09/12.
//
//

#import "DisassembledTableController.h"
#import "Breakpoint.h"
#import "DebuggerBridge.h"

@interface DisassembledTableController ()

@end

#define DISASSEMBLY_WINDOW_SIZE 200  // display at most 200 instructions in the window

@implementation DisassembledTableController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
        _disassembled = [[NSMutableArray alloc] initWithCapacity:DISASSEMBLY_WINDOW_SIZE];
    }

    return self;
}

#pragma mark -
#pragma mark public methods

- (void)setDebugger:(DebuggerBridge *)debugger {
    _debugger = debugger;
}

#define DISASSEMBLY_WINDOW_AHEAD 50  // display at least 50 instructions ahead of pc
#define MAX_INSTRUCTION_LENGTH 3     // it takes at most 3 bytes to store an instruction

- (void)updateWithPc:(NSUInteger)pc
{
    // fill up the disassembled opcode table view if we are running out of current
    // window
    [_disassembledView beginUpdates];
    
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
            [_disassembled addObject:[_debugger disassemble:&lastAddr]];
        }
        NSRange range = {prevCount, _disassembled.count - prevCount};
        indexesAdded = [NSIndexSet indexSetWithIndexesInRange:range];
    }
    
    // if the window is too large, shrink it.
    NSIndexSet *indexesRemoved = nil;
    if (_disassembled.count > DISASSEMBLY_WINDOW_SIZE) {
        NSUInteger currentIndex = [self indexOfOpcodeAtAddress:pc];
        NSUInteger lastIndex = _disassembled.count - DISASSEMBLY_WINDOW_SIZE;
        NSRange range = {0, MIN(currentIndex, lastIndex)};
        if (range.length) {
            [_disassembled removeObjectsInRange:range];
            indexesRemoved = [NSIndexSet indexSetWithIndexesInRange:range];
        }
    }
    if (indexesAdded) {
        [_disassembledView insertRowsAtIndexes:indexesAdded
                                     withAnimation:(NSTableViewAnimationEffectFade|
                                                    NSTableViewAnimationSlideUp)];
    }
    if (indexesRemoved) {
        [_disassembledView removeRowsAtIndexes:indexesRemoved
                                     withAnimation:(NSTableViewAnimationEffectFade|
                                                    NSTableViewAnimationSlideUp)];
    }
    [_disassembledView endUpdates];
    
    // highlight current instruction
    NSUInteger currentIndex = [self indexOfOpcodeAtAddress:pc];
    NSAssert(currentIndex != NSNotFound, @"current instruction %ld not in window", pc);
    NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:currentIndex];
    [_disassembledView scrollRowToVisible:currentIndex];
    [[_disassembledView animator] selectRowIndexes:indexSet byExtendingSelection:NO];
}

#pragma mark -
#pragma mark NSTableView delegate/datasource methods
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return _disassembled.count;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    Decoded *decoded = _disassembled[row];
    // assuming the identifiers of the columns are the same as the propertie names
    // of class Opcode
    return [decoded valueForKey:tableColumn.identifier];
}

#pragma mark -
#pragma mark private methods

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

@end
