//
//  WatchTableController.m
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import "WatchTableController.h"
#import "Watched.h"

@interface WatchTableController ()

@end

@implementation WatchTableController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        _watches = [[NSMutableArray alloc] initWithCapacity:20];
        for (NSString *name in @[@"%pc", @"%A", @"%X", @"%Y", @"%sp", @"%P"]) {
            [self addWatch:name];
        }
    }
    return self;
}

#pragma mark -
#pragma mark public methods

- (void)setDebugger:(DebuggerBridge *)debugger {
    _debugger = debugger;
}

- (void)update {
    NSMutableIndexSet *rowIndexSet = [NSMutableIndexSet indexSet];
    NSUInteger index = 0;
    for (Watched *watched in _watches) {
        if ([watched update:_debugger]) {
            [rowIndexSet addIndex:index];
        }
        index++;
    }
    if (rowIndexSet.count) {
        [_watchesView reloadDataForRowIndexes:rowIndexSet
                                columnIndexes:[NSIndexSet indexSetWithIndex:1]];
    }
}

- (void)addWatch:(NSString *)name {
    NSUInteger index = [self indexOfWatchWithName:name];
    if (index != NSNotFound)
        return;
    [_watches addObject:[Watched watchedWithName:name]];
    [_watchesView insertRowsAtIndexes:[NSIndexSet indexSetWithIndex:_watches.count - 1]
                        withAnimation:(NSTableViewAnimationEffectFade|
                                       NSTableViewAnimationSlideUp)];
}

- (void)removeWatch:(NSString *)name {
    NSUInteger index = [self indexOfWatchWithName:name];
    if (index == NSNotFound)
        return;
    [_watches removeObjectAtIndex:index];
    [_watchesView removeRowsAtIndexes:[NSIndexSet indexSetWithIndex:index]
                        withAnimation:(NSTableViewAnimationEffectFade|
                                       NSTableViewAnimationSlideDown)];
}

#pragma mark -
#pragma mark private methods

- (NSUInteger)indexOfWatchWithName:(NSString *)name {
    NSUInteger index = [_watches indexOfObjectPassingTest:^(Watched *obj,
                                                            NSUInteger index,
                                                            BOOL *stop) {
        if ([obj.name isEqualToString:name]) {
            *stop = YES;
            return YES;
        } else {
            return NO;
        }
    }];
    return index;
}

#pragma mark -
#pragma mark NSTableView delegate/datasource methods

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return _watches.count;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    Watched *watched = _watches[row];
    // assuming the identifiers of the columns are the same as the propertie names
    // of class Opcode
    return [watched valueForKey:tableColumn.identifier];
}

@end
