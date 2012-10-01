//
//  Watched.h
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import <Foundation/Foundation.h>

@class DebuggerBridge;

@interface Watched : NSObject {
    NSString *(^_getValue)(DebuggerBridge *);
}

+ (Watched *)watchedWithName:(NSString *)name;
- (BOOL)update:(DebuggerBridge *)debugger;

@property(copy) NSString *name;
@property(copy) NSString *value;
@property(copy) NSString *format;

@end
