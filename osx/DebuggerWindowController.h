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
{
    IBOutlet DisassembledTableView *disassembledView;
    IBOutlet WatchTableView *watchView;
    IBOutlet DebugConsoleView *consoleView;
}

@property(nonatomic, strong) DebuggerBridge* debugger;

- (id)initWithEmu:(void *)emu;

@end
