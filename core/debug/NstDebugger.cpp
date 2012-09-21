//
//  NstDisassembler.cpp
//  OpenNestopia
//
//  Created by Kefu Chai on 09/09/12.
//
//

#include "NstDebugger.h"
#include "NstOpcode.h"
#include "NstOperator.h"
#include "NstCpu.hpp"
#include "NstMachine.hpp"

#include "DebuggerDelegateBridge.h"

namespace {

    using namespace Debug;
    using Debug::NONE;
    using Debug::READ;
    using Debug::WRITE;
    using Debug::RW;

#define NES_I____(instr_, hex_)                     \
    static Opcode_<instr_, Implied, NONE> op##hex_;

#define NES____C_(instr_, ticks_, hex_)             \
    static Opcode_<instr_, Implied, NONE> op##hex_;

// used by branch instructions exclusively
#define NES_I___N(instr_, addr_, hex_)          \
    static Opcode_<instr_, addr_, NONE> op##hex_;

#define NES_IR___(instr_, addr_, hex_)              \
    static Opcode_<instr_, addr_, READ> op##hex_;
    
#define NES_I_W__(instr_, addr_, hex_)              \
    static Opcode_<instr_, addr_, WRITE> op##hex_;
    
#define NES_IRW__(instr_, addr_, hex_)          \
    static Opcode_<instr_, addr_, RW> op##hex_;
    
    // param 1 = instruction
    // param 2 = addressing mode
    // param 3 = cycles
    // param 4 = opcode
    NES_IR___( Adc, Imm,      0x69 )
    NES_IR___( Adc, Zpg,      0x65 )
    NES_IR___( Adc, ZpgX,     0x75 )
    NES_IR___( Adc, Abs,      0x6D )
    NES_IR___( Adc, AbsX,     0x7D )
    NES_IR___( Adc, AbsY,     0x79 )
    NES_IR___( Adc, IndX,     0x61 )
    NES_IR___( Adc, IndY,     0x71 )
    NES_IR___( And, Imm,      0x29 )
    NES_IR___( And, Zpg,      0x25 )
    NES_IR___( And, ZpgX,     0x35 )
    NES_IR___( And, Abs,      0x2D )
    NES_IR___( And, AbsX,     0x3D )
    NES_IR___( And, AbsY,     0x39 )
    NES_IR___( And, IndX,     0x21 )
    NES_IR___( And, IndY,     0x31 )
    NES_IRW__( Asl, Acc,      0x0A )
    NES_IRW__( Asl, Zpg,      0x06 )
    NES_IRW__( Asl, ZpgX,     0x16 )
    NES_IRW__( Asl, Abs,      0x0E )
    NES_IRW__( Asl, AbsX,     0x1E )
    NES_I___N( Bcc, Rel,      0x90 )
    NES_I___N( Bcs, Rel,      0xB0 )
    NES_I___N( Beq, Rel,      0xF0 )
    NES_IR___( Bit, Zpg,      0x24 )
    NES_IR___( Bit, Abs,      0x2C )
    NES_I___N( Bmi, Rel,      0x30 )
    NES_I___N( Bne, Rel,      0xD0 )
    NES_I___N( Bpl, Rel,      0x10 )
    NES_I___N( Bvc, Rel,      0x50 )
    NES_I___N( Bvs, Rel,      0x70 )
    NES_I____( Clc,           0x18 )
    NES_I____( Cld,           0xD8 )
    NES_I____( Cli,           0x58 )
    NES_I____( Clv,           0xB8 )
    NES_IR___( Cmp, Imm,      0xC9 )
    NES_IR___( Cmp, Zpg,      0xC5 )
    NES_IR___( Cmp, ZpgX,     0xD5 )
    NES_IR___( Cmp, Abs,      0xCD )
    NES_IR___( Cmp, AbsX,     0xDD )
    NES_IR___( Cmp, AbsY,     0xD9 )
    NES_IR___( Cmp, IndX,     0xC1 )
    NES_IR___( Cmp, IndY,     0xD1 )
    NES_IR___( Cpx, Imm,      0xE0 )
    NES_IR___( Cpx, Zpg,      0xE4 )
    NES_IR___( Cpx, Abs,      0xEC )
    NES_IR___( Cpy, Imm,      0xC0 )
    NES_IR___( Cpy, Zpg,      0xC4 )
    NES_IR___( Cpy, Abs,      0xCC )
    NES_IRW__( Dec, Zpg,      0xC6 )
    NES_IRW__( Dec, ZpgX,     0xD6 )
    NES_IRW__( Dec, Abs,      0xCE )
    NES_IRW__( Dec, AbsX,     0xDE )
    NES_I____( Dex,           0xCA )
    NES_I____( Dey,           0x88 )
    NES_IR___( Eor, Imm,      0x49 )
    NES_IR___( Eor, Zpg,      0x45 )
    NES_IR___( Eor, ZpgX,     0x55 )
    NES_IR___( Eor, Abs,      0x4D )
    NES_IR___( Eor, AbsX,     0x5D )
    NES_IR___( Eor, AbsY,     0x59 )
    NES_IR___( Eor, IndX,     0x41 )
    NES_IR___( Eor, IndY,     0x51 )
    NES_IRW__( Inc, Zpg,      0xE6 )
    NES_IRW__( Inc, ZpgX,     0xF6 )
    NES_IRW__( Inc, Abs,      0xEE )
    NES_IRW__( Inc, AbsX,     0xFE )
    NES_I____( Inx,           0xE8 )
    NES_I____( Iny,           0xC8 )
    NES_IR___( Jmp, Abs,      0x4C )
    NES_IR___( Jmp, Ind,      0x6C )
    NES_I___N( Jsr, Abs,      0x20 )
    NES_IR___( Lda, Imm,      0xA9 )
    NES_IR___( Lda, Zpg,      0xA5 )
    NES_IR___( Lda, ZpgX,     0xB5 )
    NES_IR___( Lda, Abs,      0xAD )
    NES_IR___( Lda, AbsX,     0xBD )
    NES_IR___( Lda, AbsY,     0xB9 )
    NES_IR___( Lda, IndX,     0xA1 )
    NES_IR___( Lda, IndY,     0xB1 )
    NES_IR___( Ldx, Imm,      0xA2 )
    NES_IR___( Ldx, Zpg,      0xA6 )
    NES_IR___( Ldx, ZpgY,     0xB6 )
    NES_IR___( Ldx, Abs,      0xAE )
    NES_IR___( Ldx, AbsY,     0xBE )
    NES_IR___( Ldy, Imm,      0xA0 )
    NES_IR___( Ldy, Zpg,      0xA4 )
    NES_IR___( Ldy, ZpgX,     0xB4 )
    NES_IR___( Ldy, Abs,      0xAC )
    NES_IR___( Ldy, AbsX,     0xBC )
    NES_IRW__( Lsr, Acc,      0x4A )
    NES_IRW__( Lsr, Zpg,      0x46 )
    NES_IRW__( Lsr, ZpgX,     0x56 )
    NES_IRW__( Lsr, Abs,      0x4E )
    NES_IRW__( Lsr, AbsX,     0x5E )
    NES____C_( Nop,        2, 0x1A )
    NES____C_( Nop,        2, 0x3A )
    NES____C_( Nop,        2, 0x5A )
    NES____C_( Nop,        2, 0x7A )
    NES____C_( Nop,        2, 0xDA )
    NES____C_( Nop,        2, 0xEA )
    NES____C_( Nop,        2, 0xFA )
    NES_IR___( Ora, Imm,      0x09 )
    NES_IR___( Ora, Zpg,      0x05 )
    NES_IR___( Ora, ZpgX,     0x15 )
    NES_IR___( Ora, Abs,      0x0D )
    NES_IR___( Ora, AbsX,     0x1D )
    NES_IR___( Ora, AbsY,     0x19 )
    NES_IR___( Ora, IndX,     0x01 )
    NES_IR___( Ora, IndY,     0x11 )
    NES_I____( Pha,           0x48 )
    NES_I____( Php,           0x08 )
    NES_I____( Pla,           0x68 )
    NES_I____( Plp,           0x28 )
    NES_IRW__( Rol, Acc,      0x2A )
    NES_IRW__( Rol, Zpg,      0x26 )
    NES_IRW__( Rol, ZpgX,     0x36 )
    NES_IRW__( Rol, Abs,      0x2E )
    NES_IRW__( Rol, AbsX,     0x3E )
    NES_IRW__( Ror, Acc,      0x6A )
    NES_IRW__( Ror, Zpg,      0x66 )
    NES_IRW__( Ror, ZpgX,     0x76 )
    NES_IRW__( Ror, Abs,      0x6E )
    NES_IRW__( Ror, AbsX,     0x7E )
    NES_I____( Rti,           0x40 )
    NES_I____( Rts,           0x60 )
    NES_IR___( Sbc, Imm,      0xE9 )
    NES_IR___( Sbc, Imm,      0xEB )
    NES_IR___( Sbc, Zpg,      0xE5 )
    NES_IR___( Sbc, ZpgX,     0xF5 )
    NES_IR___( Sbc, Abs,      0xED )
    NES_IR___( Sbc, AbsX,     0xFD )
    NES_IR___( Sbc, AbsY,     0xF9 )
    NES_IR___( Sbc, IndX,     0xE1 )
    NES_IR___( Sbc, IndY,     0xF1 )
    NES_I____( Sec,           0x38 )
    NES_I____( Sed,           0xF8 )
    NES_I____( Sei,           0x78 )
    NES_I_W__( Sta, Zpg,      0x85 )
    NES_I_W__( Sta, ZpgX,     0x95 )
    NES_I_W__( Sta, Abs,      0x8D )
    NES_I_W__( Sta, AbsX,     0x9D )
    NES_I_W__( Sta, AbsY,     0x99 )
    NES_I_W__( Sta, IndX,     0x81 )
    NES_I_W__( Sta, IndY,     0x91 )
    NES_I_W__( Stx, Zpg,      0x86 )
    NES_I_W__( Stx, ZpgY,     0x96 )
    NES_I_W__( Stx, Abs,      0x8E )
    NES_I_W__( Sty, Zpg,      0x84 )
    NES_I_W__( Sty, ZpgX,     0x94 )
    NES_I_W__( Sty, Abs,      0x8C )
    NES_I____( Tax,           0xAA )
    NES_I____( Tay,           0xA8 )
    NES_I____( Tsx,           0xBA )
    NES_I____( Txa,           0x8A )
    NES_I____( Txs,           0x9A )
    NES_I____( Tya,           0x98 )
    NES_I____( Brk,           0x00 )

    NES_I____( Doh,           0xFF )

#undef NES_I____
#undef NES____C_
#undef NES_IR___
#undef NES_I_W__
#undef NES_IRW__

    Opcode* opcodes[0x100] = {
        &op0x00, &op0x01, &op0xFF, &op0xFF,
        &op0xFF, &op0x05, &op0x06, &op0xFF,
        &op0x08, &op0x09, &op0x0A, &op0xFF,
        &op0xFF, &op0x0D, &op0x0E, &op0xFF,
        &op0x10, &op0x11, &op0xFF, &op0xFF,
        &op0xFF, &op0x15, &op0x16, &op0xFF,
        &op0x18, &op0x19, &op0x1A, &op0xFF,
        &op0xFF, &op0x1D, &op0x1E, &op0xFF,
        &op0x20, &op0x21, &op0xFF, &op0xFF,
        &op0x24, &op0x25, &op0x26, &op0xFF,
        &op0x28, &op0x29, &op0x2A, &op0xFF,
        &op0x2C, &op0x2D, &op0x2E, &op0xFF,
        &op0x30, &op0x31, &op0xFF, &op0xFF,
        &op0xFF, &op0x35, &op0x36, &op0xFF,
        &op0x38, &op0x39, &op0x3A, &op0xFF,
        &op0xFF, &op0x3D, &op0x3E, &op0xFF,
        &op0x40, &op0x41, &op0xFF, &op0xFF,
        &op0xFF, &op0x45, &op0x46, &op0xFF,
        &op0x48, &op0x49, &op0x4A, &op0xFF,
        &op0x4C, &op0x4D, &op0x4E, &op0xFF,
        &op0x50, &op0x51, &op0xFF, &op0xFF,
        &op0xFF, &op0x55, &op0x56, &op0xFF,
        &op0x58, &op0x59, &op0x5A, &op0xFF,
        &op0xFF, &op0x5D, &op0x5E, &op0xFF,
        &op0x60, &op0x61, &op0xFF, &op0xFF,
        &op0xFF, &op0x65, &op0x66, &op0xFF,
        &op0x68, &op0x69, &op0x6A, &op0xFF,
        &op0x6C, &op0x6D, &op0x6E, &op0xFF,
        &op0x70, &op0x71, &op0xFF, &op0xFF,
        &op0xFF, &op0x75, &op0x76, &op0xFF,
        &op0x78, &op0x79, &op0x7A, &op0xFF,
        &op0xFF, &op0x7D, &op0x7E, &op0xFF,
        &op0xFF, &op0x81, &op0xFF, &op0xFF,
        &op0x84, &op0x85, &op0x86, &op0xFF,
        &op0x88, &op0xFF, &op0x8A, &op0xFF,
        &op0x8C, &op0x8D, &op0x8E, &op0xFF,
        &op0x90, &op0x91, &op0xFF, &op0xFF,
        &op0x94, &op0x95, &op0x96, &op0xFF,
        &op0x98, &op0x99, &op0x9A, &op0xFF,
        &op0xFF, &op0x9D, &op0xFF, &op0xFF,
        &op0xA0, &op0xA1, &op0xA2, &op0xFF,
        &op0xA4, &op0xA5, &op0xA6, &op0xFF,
        &op0xA8, &op0xA9, &op0xAA, &op0xFF,
        &op0xAC, &op0xAD, &op0xAE, &op0xFF,
        &op0xB0, &op0xB1, &op0xFF, &op0xFF,
        &op0xB4, &op0xB5, &op0xB6, &op0xFF,
        &op0xB8, &op0xB9, &op0xBA, &op0xFF,
        &op0xBC, &op0xBD, &op0xBE, &op0xFF,
        &op0xC0, &op0xC1, &op0xFF, &op0xFF,
        &op0xC4, &op0xC5, &op0xC6, &op0xFF,
        &op0xC8, &op0xC9, &op0xCA, &op0xFF,
        &op0xCC, &op0xCD, &op0xCE, &op0xFF,
        &op0xD0, &op0xD1, &op0xFF, &op0xFF,
        &op0xFF, &op0xD5, &op0xD6, &op0xFF,
        &op0xD8, &op0xD9, &op0xDA, &op0xFF,
        &op0xFF, &op0xDD, &op0xDE, &op0xFF,
        &op0xE0, &op0xE1, &op0xFF, &op0xFF,
        &op0xE4, &op0xE5, &op0xE6, &op0xFF,
        &op0xE8, &op0xE9, &op0xEA, &op0xEB,
        &op0xEC, &op0xED, &op0xEE, &op0xFF,
        &op0xF0, &op0xF1, &op0xFF, &op0xFF,
        &op0xFF, &op0xF5, &op0xF6, &op0xFF,
        &op0xF8, &op0xF9, &op0xFA, &op0xFF,
        &op0xFF, &op0xFD, &op0xFE, &op0xFF
    };
}

namespace Debug {
    Debugger::Debugger(Nes::Core::Machine& machine)
    : cpu_(machine.cpu)
    {}

    uint8_t
    Debugger::peek8(uint16_t addr)
    {
        return cpu_.map.Peek8(addr);
    }

    void
    Debugger::poke8(uint16_t addr, uint8_t data)
    {
        cpu_.map.Poke8(addr, data);
    }

    uint16_t
    Debugger::peek16(uint16_t addr)
    {
        return cpu_.map.Peek16(addr);
    }

    uint8_t
    Debugger::peek_reg(Reg::All reg)
    {
        switch (reg) {
            case Reg::A:
                return cpu_.a;
            case Reg::X:
                return cpu_.x;
            case Reg::Y:
                return cpu_.y;
            case Reg::P:
                return cpu_.flags.Pack();
            case Reg::SP:
                return cpu_.sp;
            case Reg::PC:
                return cpu_.pc;
            default:
                assert(0);
        }
    }

    void
    Debugger::poke_reg(Reg::All reg, uint8_t data)
    {
        switch (reg) {
            case Reg::A:
                cpu_.a = data;
                break;
            case Reg::X:
                cpu_.x = data;
                break;
            case Reg::Y:
                cpu_.y = data;
                break;
            case Reg::P:
                // just for the sake of completeness
                cpu_.flags.Unpack(data);
                break;
            case Reg::SP:
                cpu_.sp = data;
                break;
            case Reg::PC:
                cpu_.pc = data;
                break;
            default:
                assert(0);
        }
    }

    int
    Debugger::set_breakpoint(uint16_t addr, AccessMode mode)
    {
        return bpm_.set(addr, mode);
    }

    void
    Debugger::remove_breakpoint(int index)
    {
        return bpm_.remove(index);
    }

    const Breakpoint*
    Debugger::lookup_breakpoint(int index)
    {
        return bpm_.lookup(index);
    }

    void
    Debugger::cpu_op_exec(uint16_t pc)
    {
        int bp_index = bpm_.test_access(Access({pc, EXEC}));
        if (bp_index > 0) {
            return delegate_->suspend_at(bp_index, pc);
        }
        Opcode* opcode = opcodes[cpu_.map.Peek8(pc)];
        bp_index = bpm_.test_access(opcode->access(pc));
        if (bp_index > 0) {
            return delegate_->suspend_at(pc, bp_index);
        }
    }

    Decoded
    Debugger::disassemble(uint16_t& pc) {
        uint hex = cpu_.map.Peek8(pc++);
        return opcodes[hex]->decode(pc);
    }
}
