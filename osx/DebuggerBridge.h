//
//  DebuggerBridge.h
//  OpenNestopia
//
//  Created by Kefu Chai on 16/09/12.
//
//

#import <Foundation/Foundation.h>

typedef enum : char {
    A = 'A',
    X = 'X',
    Y = 'Y',
    S = 'S',
    M = 'M',
    P = 'P',
    SP = 'S',
    PC = 'C',
} Reg;

typedef enum : NSUInteger {
    AccessNone  = 0,
    AccessRead  = 1 << 0,
    AccessWrite = 1 << 1,
    AccessRW = AccessRead | AccessWrite
} AccessMode;

@interface Register : NSObject
+ (Reg)regWithName:(NSString*)name;
+ (NSString *)nameWithReg:(Reg)reg;
@end

@interface DebuggerBridge : NSObject

- (id)initWithEmu:(void *)emu;
- (uint8_t)peek8:(uint16_t)addr;
- (void)poke8:(uint16_t)addr with:(uint8_t)data;
- (uint8_t)peekReg:(Reg)reg;
- (void)pokeReg:(Reg)reg with:(uint8_t)data;
- (int)setBreakpointAt:(uint16_t)pc;
- (int)stopAt:(uint16_t)pc whenAddress:(uint16_t)addr is:(AccessMode)access;
- (void)resetBreakpoint:(int)breakpoint;


@end
