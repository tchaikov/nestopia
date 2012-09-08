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
        static const Addr_& get() {
            static Addr_ instance;
            return instance;
        }
    protected:
        uint fetchPc8(uint &pc) const {
            const uint address = cpu_->map.Peek16(pc);
            ++pc;
            return address;
        }
        uint fetchPc16(uint &pc) const {
            const uint address = cpu_->map.Peek16(pc);
            pc += 2;
            return address;
        }
    private:
        const Nes::Core::Cpu* cpu_;
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
        uint fetch(uint&) const {
            // never reach here
            BOOST_ASSERT(false);
        }
    };
    
    // immediate addressing
    struct Imm : Addr<Imm> {
        // should be LDA #10 ; load 10 ($0A) into the accumulator
        // but i prefer the hex presentation
        Imm() : Addr<Imm>("#$%02X") {}
        uint fetch(uint& pc) const {
            return fetchPc8(pc);
        }
    };
    
    // absolute addressing
    struct Abs : Addr<Abs> {
        Abs() : Addr<Abs>("$%04X") {}
        uint fetch(uint& pc) const {
            return fetchPc16(pc);
        }
    };
    
    // Zero page addressing
    struct Zpg : Addr<Zpg> {
        Zpg() : Addr<Zpg>("$%02X") {}
        uint fetch(uint &pc) const {
            return fetchPc16(pc);
        }
    };
    
    // Zero page indexed addressing (X or Y)
    template<Reg::Index::All reg>
    struct ZeroPageIndexed : Addr<ZeroPageIndexed<reg> > {
        // LDA $3F, X
        ZeroPageIndexed()
        : Addr<ZeroPageIndexed<reg> >(format("$2%2$02X, %1$C") % reg) {}
        uint fetch(uint &pc) const {
            return Addr<ZeroPageIndexed<reg> >::fetchPc8(pc);
        }
    };
    typedef ZeroPageIndexed<Reg::Index::X> ZpgX;
    typedef ZeroPageIndexed<Reg::Index::Y> ZpgY;
    
    // Absolute indexed addressing
    template<Reg::Index::All reg>
    struct AbsIndexed : Addr<AbsIndexed<reg> > {
        // LDA $8000, X
        AbsIndexed()
        : Addr<AbsIndexed<reg> >(format("$2%2$04X, %1$C") % reg) {}
        uint fetch(uint &pc) const {
            return Addr<AbsIndexed<reg> >::fetchPc16(pc);
        }
    };
    typedef AbsIndexed<Reg::Index::X> AbsX;
    typedef AbsIndexed<Reg::Index::Y> AbsY;
    
    // Indexed indirect addressing
    template<Reg::Index::All reg>
    struct IndexedIndirect : Addr<IndexedIndirect<reg> > {
        IndexedIndirect()
        : Addr<IndexedIndirect<reg> >(format("($2%2$02X, %1$C)") % reg) {}
        uint fetch(uint &pc) const {
            return Addr<IndexedIndirect<reg> >::fetchPc8(pc);
        }
    };
    typedef IndexedIndirect<Reg::Index::X> IndX;

    // Indirect Indexed addressing
    template<Reg::Index::All reg>
    struct IndirectIndexed : Addr<IndirectIndexed<reg> > {
        IndirectIndexed()
        : Addr<IndirectIndexed<reg> >(format("($2%2$02X), %1$C)") % reg) {}
        uint fetch(uint &pc) const {
            return Addr<IndirectIndexed<reg> >::fetchPc8(pc);
        }
    };
    typedef IndirectIndexed<Reg::Index::Y> IndY;

    // Indirect addressing
    struct Ind : Addr<Ind> {
        Ind() : Addr<Ind>("($04X)") {}
        uint fetch(uint &pc) const {
            return fetchPc16(pc);
        }
    };

    // Relative addressing
    struct Rel : Addr<Rel> {
        Rel() : Addr<Rel>("$+02X") {}
        uint fetch(uint& pc) const {
            return fetchPc8(pc);
        }
    };
}

