#import <Foundation/Foundation.h>
#import "CommandRunner.h"

@class Breakpoint;
@class CommandRunner;

@interface CommandParser : NSObject {
    Breakpoint *_bp;
    NSData *_bytes;

    int _dec;
    int _hex;
    int _currentState;
    char *_tokenStart;
    char *_tokenEnd;
    int _act;
}

- (id)initWithRunner:(id<CommandRunner>)runner;
- (void)parse:(NSString *)command;
@property (readonly) BOOL completed;
@property (readonly) BOOL error;
@property (assign) id<CommandRunner> commandRunner;

@end

