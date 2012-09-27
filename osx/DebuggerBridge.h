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

@interface Register : NSObject
+ (Reg)regWithName:(NSString*)name;
+ (NSString *)nameWithReg:(Reg)reg;
@end

@interface Decoded : NSObject
@property (copy) NSString *description;
@property (copy) NSString *repr;
@property (assign) NSUInteger address;
@end

@class Breakpoint;

@interface DebuggerBridge : NSObject

- (id)initWithEmu:(void *)emu;

- (uint8_t)peek8:(uint16_t)addr;
- (void)poke8:(uint16_t)addr with:(uint8_t)data;
- (uint8_t)peekReg:(Reg)reg;
- (void)pokeReg:(Reg)reg with:(uint8_t)data;

- (int)setBreakpoint:(Breakpoint *)bp;
- (void)resetBreakpoint:(int)breakpoint;
- (Breakpoint *)breakpointAtIndex:(NSUInteger)index;

- (Decoded *)disassemble:(NSUInteger *)addr;

@end
