//
//  DebuggerBridge.m
//  OpenNestopia
//
//  Created by Kefu Chai on 16/09/12.
//
//

#import "DebuggerBridge.h"

#import "Breakpoint.h"
#import "Decoded.h"

#include "NstDebugger.h"
#include "NstApiEmulator.hpp"
#include "NstMachine.hpp"

@interface DebuggerBridge () {
    Debug::Debugger *debugger;
}
@end

@implementation DebuggerBridge


- (id)initWithEmu:(void *)emu
{
    self = [super init];
    if (self) {
        Nes::Core::Machine& machine(*(Nes::Api::Emulator *)emu);
        debugger = new Debug::Debugger(machine);
    }
    return self;
}

- (void)dealloc
{
    delete debugger;
}

- (uint8_t)peek8:(uint16_t)addr
{
    return debugger->peek8(addr);
}

- (void)poke8:(uint16_t)addr with:(uint8_t)data
{
    return debugger->poke8(addr, data);
}

- (uint16_t)peekReg:(Reg)reg
{
    return debugger->peek_reg((Debug::Reg::All)reg);
}

- (void)pokeReg:(Reg)reg with:(uint8_t)data
{
    debugger->poke_reg((Debug::Reg::All)reg, data);
}

- (int)setBreakpoint:(Breakpoint *)bp {
    return debugger->set_breakpoint(bp.address,
                                    (Debug::AccessMode)bp.access);
}

- (BOOL)resetBreakpoint:(int)index {
    return debugger->remove_breakpoint(index);
}

- (BOOL)disableBreakpoint:(int)index {
    return debugger->disable_breakpoint(index);
}

- (BOOL)enableBreakpoint:(int)index {
    return debugger->enable_breakpoint(index);
}

- (Breakpoint *)breakpointAtIndex:(NSUInteger)index {
    const Debug::Breakpoint* dbp = debugger->lookup_breakpoint(index);
    if (dbp == nullptr)
        return nil;
    return [[Breakpoint alloc] initWithAddress:dbp->address
                                        access:(AccessMode)dbp->access
                                       enabled:dbp->enabled];
}

- (void)next {
    @synchronized(self) {
        debugger->next();
    }
}

- (void)stepInto {
    @synchronized(self) {
        debugger->step_into();
    }
}

- (void)pause {
    @synchronized(self) {
        debugger->pause();
    }
}

- (void)resume {
    @synchronized(self) {
        debugger->resume();
    }
}

- (void)until:(NSUInteger)address {
    @synchronized(self) {
        debugger->until(address);
    }
}
- (BOOL)shouldExec {
    @synchronized(self) {
        return debugger->should_exec();
    }
}

- (Decoded *)disassemble:(NSUInteger *)addr
{
    uint16_t pc = *addr;
    Debug::Decoded decoded = debugger->disassemble(pc);
    Decoded *brDecoded = [[Decoded alloc] init];
    brDecoded.description = [NSString stringWithUTF8String:decoded.str.c_str()];
    brDecoded.repr = [NSString stringWithUTF8String:decoded.repr.c_str()];
    brDecoded.address = *addr;
    *addr = pc;
    return brDecoded;
}

@end
