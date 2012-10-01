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
@class WatchTableController;
@class DebugConsoleView;
@class DebuggerBridge;
@class NESGameCore;

@interface DebuggerWindowController : NSWindowController<DebuggerDelegate,
    NSTextViewDelegate> {
@private
    NSUInteger committedLength;

    DisassembledTableController *_disassembledController;
    IBOutlet NSView *_disassembledView;

    WatchTableController *_watchController;
    IBOutlet NSView *_watchView;
}

@property(nonatomic, strong) DebuggerBridge* debugger;

@property(assign) IBOutlet NSTextView *consoleView;
@property(nonatomic, assign) NESGameCore *gameCore;

- (void)setGameCore:(NESGameCore *)gameCore;

@end
