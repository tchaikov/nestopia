//
//  DebuggerController.h
//  OpenNestopia
//
//  Created by Kefu Chai on 04/09/12.
//
//

#import <Cocoa/Cocoa.h>

#import "DebuggerDelegate.h"
#import "CommandRunner.h"

@class DisassembledTableController;
@class WatchTableController;
@class DebugConsoleView;
@class CommandParser;
@class DebuggerBridge;
@class NESGameCore;

@interface DebuggerWindowController : NSWindowController<DebuggerDelegate,
NSTextViewDelegate, CommandRunner> {
@private
    NSUInteger committedLength;
    CommandParser* _commandParser;
    // TODO: support $(n) variable ?
    int _printCount;

    DisassembledTableController *_disassembledController;
    IBOutlet NSView *_disassembledView;

    WatchTableController *_watchController;
    IBOutlet NSView *_watchView;
}

@property(nonatomic, strong) DebuggerBridge* debugger;
@property(nonatomic, assign) NESGameCore *gameCore;
@property(assign) IBOutlet NSTextView *consoleView;

- (void)setGameCore:(NESGameCore *)gameCore;

@end
