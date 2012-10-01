//
//  WatchTableController.h
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import <Cocoa/Cocoa.h>

@class DebuggerBridge;

@interface WatchTableController : NSViewController<NSTableViewDelegate, NSTableViewDataSource> {
@private
    NSMutableArray *_watches;
    DebuggerBridge *_debugger;

    IBOutlet NSTableView *_watchesView;
}

- (void)setDebugger:(DebuggerBridge *)debugger;
- (void)update;
- (void)addWatch:(NSString *)name;
- (void)removeWatch:(NSString *)name;


@end
