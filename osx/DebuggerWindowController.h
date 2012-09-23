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


@interface DebuggerWindowController : NSWindowController<DebuggerDelegate>

@property(nonatomic, strong) DebuggerBridge* debugger;
@property(assign) IBOutlet DisassembledTableView *disassembledView;
@property(assign) IBOutlet WatchTableView *watchView;
@property(assign) IBOutlet DebugConsoleView *consoleView;

- (void)setEmu:(void *)emu;

@end
