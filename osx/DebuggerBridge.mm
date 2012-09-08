//
//  DebuggerBridge.m
//  OpenNestopia
//
//  Created by Kefu Chai on 16/09/12.
//
//

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

- (int)setBreakpointAt:(uint16_t)pc
{
    return debugger->set_breakpoint(pc);
}

- (int)stopAt:(uint16_t)pc whenAddress:(uint16_t)addr is:(AccessMode)accessMode
{
    int index = [self setBreakpointAt:pc];
    debugger->set_condition(index, addr, (Debug::AccessMode)accessMode);
    return index;
}

- (void)resetBreakpoint:(int)index
{
    debugger->remove_breakpoint(index);
}

@end
