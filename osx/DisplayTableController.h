//
//  WatchTableController.h
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import <Cocoa/Cocoa.h>

@class DebuggerBridge;

@interface DisplayTableController : NSViewController<NSTableViewDelegate, NSTableViewDataSource> {
@private
    NSMutableArray *_watches;
    DebuggerBridge *_debugger;

    IBOutlet NSTableView *_watchesView;
}

- (void)setDebugger:(DebuggerBridge *)debugger;
- (void)update;
- (NSUInteger)addDisplay:(NSString *)name;
- (void)removeDisplay:(NSUInteger)index;


@end
