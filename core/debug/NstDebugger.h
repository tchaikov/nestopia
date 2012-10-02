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
        void next();
        void step_into();
        /// continue running until a breakpoint is reached
        void pause();
        void resume();
        void until(uint16_t address);
        /// @return true if should execute next opcode
        bool should_exec();

        Decoded disassemble(uint16_t& pc);

    private:
        bool done_with_next();
        bool done_with_step();
        bool done_with_finish();
        bool done_with_until();
        bool done_with_bp();

        int check_with_breakpoints(uint16_t pc);

    private:
        DelegateBridge *delegate_;
        Nes::Core::Cpu &cpu_;
        BreakpointManager bpm_;

        enum RunMode {
            STEP_OVER,
            STEP_INTO,
            RUN_UNTIL,
            CONTINUE,
            PAUSED,
        } run_mode_;
        bool step_done_;
        int call_depth_;
        int last_bp_;
        uint16_t until_addr_; // a single shot break point
    };
}

