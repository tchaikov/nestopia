//
//  DebuggerController.h
//  OpenNestopia
//
//  Created by Kefu Chai on 04/09/12.
//
//

#import <Cocoa/Cocoa.h>
#import "DebuggerDelegate.h"

@class DisassembledTableView;
@class WatchTableView;
@class DebugConsoleView;
@class DebuggerBridge;
@class NESGameCore;

@interface DebuggerWindowController : NSWindowController<DebuggerDelegate, NSTableViewDataSource> {
@private
    NSMutableArray *_disassembled;
}

@property(nonatomic, strong) DebuggerBridge* debugger;
@property(assign) IBOutlet NSTableView *disassembledView;
@property(assign) IBOutlet WatchTableView *watchView;
@property(assign) IBOutlet DebugConsoleView *consoleView;
@property(nonatomic, assign) NESGameCore *gameCore;

- (void)setGameCore:(NESGameCore *)gameCore;

@end
