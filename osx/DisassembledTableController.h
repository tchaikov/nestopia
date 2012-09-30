//
//  DisassembledTableController.h
//  OpenNestopia
//
//  Created by Kefu Chai on 30/09/12.
//
//

#import <Cocoa/Cocoa.h>

@class DebuggerBridge;

@interface DisassembledTableController : NSViewController<NSTableViewDelegate, NSTableViewDataSource> {

@private
    NSMutableArray *_disassembled;
    DebuggerBridge *_debugger;
}

- (void)setDebugger:(DebuggerBridge *)debugger;
- (void)updateDisassemblyWindowWithPc:(NSUInteger)pc;

@property(assign) IBOutlet NSTableView *disassembledView;


@end
