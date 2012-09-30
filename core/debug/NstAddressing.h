//
//  NstAddressing.h
//  OpenNestopia
//
//  Created by Kefu Chai on 09/09/12.
//
//

#pragma once

#include <string>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/format.hpp>

#include "../NstBase.hpp"
#include "../NstCpu.hpp"

#include "NstRegister.h"

namespace Debug {
    using boost::format;
    using std::string;
    using ::Nes::Core::Cpu;
    void set_cpu(const Cpu *cpu);
    const Cpu* get_cpu();
    /// general addressing modes
    template <class Addr_> struct Addr {
        Addr(const format& fmt)
        : fmt_(fmt) {}
        Addr(const char* fmt)
        : fmt_(fmt) {}
        /// @return the formal notation of this address in the appropiate
        ///         format.
        string str(uint data) const {
            format fmt(fmt_);
            return (fmt % data).str();
        }
        /// @return a more human-readable presentation.
        ///         for example, it resolves the indirect
        ///         address to its absolute presentation.
        string repr(uint pc, uint addr) const {
            return str(addr);
        }
        static const Addr_& instance() {
            static Addr_ addr;
            return addr;
        }

        /// get the numberic operand
        uint16_t fetch(uint16_t& pc) const;
        /// return the address to be accessed by this
        /// addressing
        /// @param the operand, it's the @c 1 in "bcc $1"
        uint16_t get(uint16_t operand) const {
            return 0;
        }
    protected:
        uint8_t peek8(uint16_t addr) const {
            return get_cpu()->map.Peek8(addr);
        }
        uint16_t peek16(uint16_t addr) const {
            return get_cpu()->map.Peek16(addr);
        }

        uint8_t fetchPc8(uint16_t &pc) const {
            const uint address = peek8(pc);
            ++pc;
            return address;
        }
        uint16_t fetchPc16(uint16_t &pc) const {
            const uint16_t address = peek16(pc);
            pc += 2;
            return address;
        }
        uint8_t readReg(Reg::Index::All reg) const {
            return readReg((Reg::All)reg);
        }
        uint8_t readReg(Reg::All reg) const {
            switch (reg) {
                case Reg::A:
                    return get_cpu()->a;
                case Reg::X:
                    return get_cpu()->x;
                case Reg::Y:
                    return get_cpu()->y;
                case Reg::SP:
                    return get_cpu()->sp;
                case Reg::PC:
                    return get_cpu()->pc;
                default:
                    return 0xFF;
            }
        }
    private:
        format fmt_;
    };

    // Implied addressing
    // since implied addressing can involve more than one register and/or
    // memory address, it needs more tweak if it is to be well presented.
    struct Implied : Addr<Implied> {
    };
    // Accumulator addressing
    struct Acc : Addr<Acc> {
        Acc() : Addr<Acc>("A") {}
        string str() const {
            return "A";
        }

        uint16_t fetch(uint16_t&) const {
            // never reach here
            BOOST_ASSERT(false);
        }
    };
    
    // immediate addressing
    struct Imm : Addr<Imm> {
        // should be LDA #10 ; load 10 ($0A) into the accumulator
        // but i prefer the hex presentation
        Imm() : Addr<Imm>("#$%02X") {}
        uint16_t fetch(uint16_t& pc) const {
            return fetchPc8(pc);
        }
    };
    
    // absolute addressing
    struct Abs : Addr<Abs> {
        Abs() : Addr<Abs>("$%04X") {}
        uint16_t fetch(uint16_t& pc) const {
            return fetchPc16(pc);
        }
        uint8_t get(uint16_t data) const {
            return data;
        }
    };
    
    // Zero page addressing
    struct Zpg : Addr<Zpg> {
        Zpg() : Addr<Zpg>("$%02X") {}
        uint16_t fetch(uint16_t &pc) const {
            return fetchPc16(pc);
        }
        uint16_t get(uint16_t data) const {
            return data & 0xFF;
        }
    };

    // Zero page indexed addressing (X or Y)
    template<Reg::Index::All reg>
    struct ZeroPageIndexed : Addr<ZeroPageIndexed<reg> > {
        // LDA $3F, X
        ZeroPageIndexed()
        : Addr<ZeroPageIndexed<reg> >(format("$%2$02X, %1$C") % reg) {}
        uint16_t fetch(uint16_t &pc) const {
            return Addr<ZeroPageIndexed<reg> >::fetchPc8(pc);
        }
        uint16_t get(uint16_t data) const {
            return (data + Addr<ZeroPageIndexed<reg> >::readReg(reg)) & 0xFF;
        }
    };
    typedef ZeroPageIndexed<Reg::Index::X> ZpgX;
    typedef ZeroPageIndexed<Reg::Index::Y> ZpgY;
    
    // Absolute indexed addressing
    template<Reg::Index::All reg>
    struct AbsIndexed : Addr<AbsIndexed<reg>> {
        // LDA $8000, X
        AbsIndexed()
        : Addr<AbsIndexed<reg>>(format("$%2$04X, %1$C") % reg) {}
        uint16_t fetch(uint16_t &pc) const {
            return Addr<AbsIndexed<reg> >::fetchPc16(pc);
        }
        uint16_t get(uint16_t data) const {
            return (data + Addr<AbsIndexed<reg>>::readReg(reg)) & 0xFFFF;
        }
    };
    typedef AbsIndexed<Reg::Index::X> AbsX;
    typedef AbsIndexed<Reg::Index::Y> AbsY;
    
    // Indexed indirect addressing
    template<Reg::Index::All reg>
    struct IndexedIndirect : Addr<IndexedIndirect<reg>> {
        IndexedIndirect()
        : Addr<IndexedIndirect<reg> >(format("($%2$02X, %1$C)") % reg) {}
        uint16_t fetch(uint16_t &pc) const {
            return Addr<IndexedIndirect<reg> >::fetchPc8(pc);
        }
        uint16_t get(uint16_t index) const {
            uint8_t offset = Addr<IndexedIndirect<reg>>::readReg(reg);
            uint16_t addr = Addr<IndexedIndirect<reg>>::peek16(index + offset);
            return Addr<IndexedIndirect<reg> >::peek8(addr);
        }
    };
    typedef IndexedIndirect<Reg::Index::X> IndX;

    // Indirect Indexed addressing
    template<Reg::Index::All reg>
    struct IndirectIndexed : Addr<IndirectIndexed<reg> > {
        IndirectIndexed()
        : Addr<IndirectIndexed<reg> >(format("($%2$02X), %1$C)") % reg) {}
        uint16_t fetch(uint16_t &pc) const {
            return Addr<IndirectIndexed<reg> >::fetchPc8(pc);
        }
        uint16_t get(uint16_t data) const {
            uint16_t index = Addr<IndirectIndexed<reg>>::peek16(data);
            uint8_t offset = Addr<IndirectIndexed<reg>>::readReg(reg);
            return Addr<IndirectIndexed<reg>>::peek8(index + offset);
        }
    };
    typedef IndirectIndexed<Reg::Index::Y> IndY;

    // Indirect addressing
    struct Ind : Addr<Ind> {
        Ind() : Addr<Ind>("($%04X)") {}
        uint16_t fetch(uint16_t &pc) const {
            return fetchPc16(pc);
        }
        uint16_t get(uint16_t data) const {
            uint16_t addr = Addr<Ind>::peek16(data);
            return Addr<Ind>::peek8(addr);
        }
    };

    // Relative addressing
    struct Rel : Addr<Rel> {
        Rel() : Addr<Rel>("$%+02X") {}
        uint16_t fetch(uint16_t& pc) const {
            return fetchPc8(pc);
        }
        uint16_t get(uint16_t data) const {
            uint16_t base = Addr<Rel>::readReg(Reg::PC) + 1;
            return (base + (int8_t)data) & 0xFFFF;
        }
    };

}

