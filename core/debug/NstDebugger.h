//
//  NstDisassembler.h
//  OpenNestopia
//
//  Created by Kefu Chai on 09/09/12.
//
//

#pragma once

#include "NstCore.hpp"
#include "NstHook.hpp"
#include "NstOpcode.h"
#include "NstRegister.h"
#include "NstBreakpoint.h"


namespace Nes {
    namespace Core {
        class Cpu;
        class Machine;
    }
}

namespace Debug {
    class DelegateBridge;

    /// façade of
    ///    - Nes::Core::Cpu, which is able to peek and poke all corners
    ///      of 6502 and the cartridge, disassemble the instructions.
    ///    - breakpoint manager
    class Debugger {
    public:
        Debugger(Nes::Core::Machine& machine);

        uint8_t peek8(uint16_t addr);
        void poke8(uint16_t addr, uint8_t data);
        uint16_t peek16(uint16_t addr);
        void poke16(uint16_t addr, uint16_t data);
        uint16_t peek_reg(Reg::All reg);
        void poke_reg(Reg::All reg, uint8_t data);

        int set_breakpoint(uint16_t pc, AccessMode access);
        bool remove_breakpoint(int index);
        bool disable_breakpoint(int index);
        bool enable_breakpoint(int index);
        const Breakpoint* lookup_breakpoint(int index);

        // cpu
        void cpu_op_exec(uint16_t addr);
        
        /// start running until a breakpoint is reached
        void resume();
        /// stop running as soon as possible
        void suspend();
        Decoded disassemble(uint16_t& pc);

    private:
        void attach();
        void detach();

        int check_with_breakpoints(uint16_t pc);

        NES_DECL_HOOK(checkNextOpcode);

    private:
        enum RunMode {
            RUN_UNTIL,
            STEP_OVER,
            STEP_INTO,
        } run_mode_;
        DelegateBridge *delegate_;
        Nes::Core::Cpu &cpu_;
        BreakpointManager bpm_;
    };
}

