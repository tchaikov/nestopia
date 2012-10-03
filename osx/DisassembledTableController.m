//
//  DisassembledTableController.m
//  OpenNestopia
//
//  Created by Kefu Chai on 30/09/12.
//
//

#import "DisassembledTableController.h"
#import "Breakpoint.h"
#import "Decoded.h"
#import "DebuggerBridge.h"


#define DISASSEMBLY_WINDOW_SIZE_MIN 200  // display at least 200 instructions in the window
#define DISASSEMBLY_WINDOW_SIZE_MAX 300  // display at most  300 instructions in the window


@implementation DisassembledTableController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
        _disassembled = [[NSMutableArray alloc] initWithCapacity:DISASSEMBLY_WINDOW_SIZE_MAX];
    }

    return self;
}

#pragma mark -
#pragma mark public methods

- (void)setDebugger:(DebuggerBridge *)debugger {
    _debugger = debugger;
}

#define DISASSEMBLY_WINDOW_AHEAD 50  // display at least 50 instructions ahead of pc

- (void)refill:(NSUInteger)pc {

    NSUInteger pcIndex = [self indexOfOpcodeAtAddress:pc];
    if (pcIndex != NSNotFound &&
        _disassembled.count - pcIndex >= DISASSEMBLY_WINDOW_SIZE_MIN)
        return;
    
    // it happens all the time, if we are not in a loop.
    if (pcIndex == NSNotFound) {
        // we are way far from current window, so reset the beginning address
        NSRange range = NSMakeRange(0, _disassembled.count);
        NSIndexSet *indexesRemoved = [NSIndexSet indexSetWithIndexesInRange:range];
        [_disassembled removeAllObjects];
        [_disassembledView removeRowsAtIndexes:indexesRemoved
                                 withAnimation:(NSTableViewAnimationEffectFade|
                                                NSTableViewAnimationSlideUp)];
    } else {
        Decoded *decoded = _disassembled.lastObject;
        pc = decoded.address;
    }

    const NSUInteger prevCount = _disassembled.count;
    NSUInteger count;
    for (count = prevCount;
         count < DISASSEMBLY_WINDOW_SIZE_MIN;
         count++) {
        [_disassembled addObject:[_debugger disassemble:&pc]];
    }
    NSRange range = NSMakeRange(prevCount, count - prevCount);
    NSIndexSet *indexesAdded = [NSIndexSet indexSetWithIndexesInRange:range];
    [_disassembledView insertRowsAtIndexes:indexesAdded
                             withAnimation:(NSTableViewAnimationEffectFade|
                                            NSTableViewAnimationSlideUp)];
}

- (void)removeExcessive:(NSUInteger)pc {

    if (_disassembled.count <= DISASSEMBLY_WINDOW_SIZE_MAX)
        return;

    NSUInteger currentIndex = [self indexOfOpcodeAtAddress:pc];
    NSUInteger lastIndex = _disassembled.count - DISASSEMBLY_WINDOW_SIZE_MAX;
    NSRange range = NSMakeRange(0, MIN(currentIndex, lastIndex));
    if (range.length == 0)
        return;

    [_disassembled removeObjectsInRange:range];
    NSIndexSet *indexesRemoved = [NSIndexSet indexSetWithIndexesInRange:range];
    [_disassembledView removeRowsAtIndexes:indexesRemoved
                             withAnimation:(NSTableViewAnimationEffectFade|
                                            NSTableViewAnimationSlideUp)];
}

- (void)updateWithPc:(NSUInteger)pc
{
    [_disassembledView beginUpdates];
    {
        // fill up the disassembled opcode table view if we are running out of current
        // window
        [self refill:pc];
        // if the window is too large, shrink it.
        [self removeExcessive:pc];
    }
    [_disassembledView endUpdates];
    
    // highlight current instruction
    NSUInteger currentIndex = [self indexOfOpcodeAtAddress:pc];
    NSAssert(currentIndex != NSNotFound, @"current instruction %ld not in window", pc);
    [_disassembledView scrollRowToVisible:currentIndex];
    [[_disassembledView animator] selectRowIndexes:[NSIndexSet indexSetWithIndex:currentIndex]
                              byExtendingSelection:NO];
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
