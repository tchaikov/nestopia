//
//  DebuggerController.h
//  OpenNestopia
//
//  Created by Kefu Chai on 04/09/12.
//
//

#import <Cocoa/Cocoa.h>
#import "DebuggerDelegate.h"

@class DisassembledTableController;
@class WatchTableView;
@class DebugConsoleView;
@class DebuggerBridge;
@class NESGameCore;

@interface DebuggerWindowController : NSWindowController<DebuggerDelegate,
    NSTextViewDelegate> {
@private
    NSUInteger committedLength;
    DisassembledTableController *_disassembledController;

}

@property(nonatomic, strong) DebuggerBridge* debugger;
@property(assign) IBOutlet NSView *disassembledView;
@property(assign) IBOutlet WatchTableView *watchView;
@property(assign) IBOutlet NSTextView *consoleView;
@property(nonatomic, assign) NESGameCore *gameCore;

- (void)setGameCore:(NESGameCore *)gameCore;

@end
