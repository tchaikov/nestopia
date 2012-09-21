//
//  DebuggerBridge.m
//  OpenNestopia
//
//  Created by Kefu Chai on 16/09/12.
//
//

#import "Breakpoint.h"
#import "DebuggerBridge.h"

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

- (uint8_t)peekReg:(Reg)reg
{
    return debugger->peek_reg((Debug::Reg::All)reg);
}

- (void)pokeReg:(Reg)reg with:(uint8_t)data
{
    debugger->poke_reg((Debug::Reg::All)reg, data);
}

- (int)setBreakpoint:(Breakpoint *)bp
{
    return debugger->set_breakpoint(bp.address,
                                    (Debug::AccessMode)bp.access);
}

- (void)resetBreakpoint:(int)index
{
    debugger->remove_breakpoint(index);
}

- (Breakpoint *)breakpointAtIndex:(NSUInteger)index
{
    const Debug::Breakpoint* dbp = debugger->lookup_breakpoint(index);
    if (dbp == nullptr)
        return nil;
    return [[Breakpoint alloc] initWithAddress:dbp->address
                                        access:(AccessMode)dbp->access
                                       enabled:dbp->enabled];
}
@end
