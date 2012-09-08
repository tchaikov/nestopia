//
//  NstDisassembler.h
//  OpenNestopia
//
//  Created by Kefu Chai on 09/09/12.
//
//

#pragma once

#include "NstOpcode.h"
#include "NstRegister.h"

#include <map>

namespace Nes {
    namespace Core {
        class Cpu;
        class Machine;
    }
}

namespace Debug {
    // processor status (the P register)
    struct Flags {
        bool carry     : 1;
        bool zero      : 1;
        bool interrupt : 1; // interrupt enabled/disable
        bool decimal   : 1; // not supported on N2A03
        bool breaks    : 1; // software interrupt,
        bool reserved  : 1; // unused, always set
        bool overflow  : 1;
        bool negative  : 1;
    };
    /// frontend of Nes::Core::Cpu, which is able to peek and poke all corners
    /// of 6502 and the cartridge, disassemble the instructions.
    class Debugger {
    public:
        Debugger(Nes::Core::Machine& machine);

        uint8_t peek8(uint16_t addr);
        void poke8(uint16_t addr, uint8_t data);
        uint16_t peek16(uint16_t addr);
        void poke16(uint16_t addr, uint16_t data);
        uint8_t peek_reg(Reg::All reg);
        void poke_reg(Reg::All reg, uint8_t data);

        int set_breakpoint(uint16_t pc);
        bool remove_breakpoint(int index);
        bool set_condition(int index, uint16_t accessed_addr, AccessMode mode);
        bool disable_breakpoint(int index);
        
        Decoded disassemble(uint& pc);
    private:
        Nes::Core::Cpu &cpu_;
        // <index, breakpoint> pair, will always use the smallest available
        // integer for the newly added breakpoint.
        typedef std::map<int, Breakpoint> Breakpoints;
        Breakpoints breakpoints_;
    };
}

