//
//  NstOperator.h
//  OpenNestopia
//
//  Created by Kefu Chai on 09/09/12.
//
//

#pragma once

#include <string>
#include <boost/format.hpp>

#include "NstRegister.h"

namespace Nes {
    namespace Core {
        class Cpu;
    }
}
namespace Debug {
    using boost::format;
    using std::string;
    
    template<class Op_>
    struct Op {
        Op(const string& op)
        : op_(op) {}
        Op(const char* op)
        : op_(op) {}
        /// @c LDA for example
        const string& str() const {
            return op_;
        }
        static const Op_& instance() {
            static const Op_ op;
            return op;
        }
    private:
        const string op_;
    };

    // N2A03 has no BCD mode, so we don't care the decimal flag
    // arithmetic ops
    struct Adc : Op<Adc> {
        Adc() : Op<Adc>("ADC") {}
        format repr(const string& m) const {
            return format("A += %1% + P.C") % m;
        }
    };
    
    struct Sbc : Op<Sbc> {
        Sbc() : Op<Sbc>("SBC") {}
        format repr(const string& m) const {
            return format("A -= %1% - !P.C") % m;
        }
    };

    // inc and dec ops
    struct Inc : Op<Inc> {
        Inc() : Op<Inc>("INC")
        {}
        format repr(const string& m) const {
            return format("%1% += 1") % m;
        }
    };
    using namespace ::Debug::Reg;
    template<Reg::Index::All reg> struct Increase : Op<Increase<reg> > {
        // well, that's nasty too.
        Increase() : Op<Increase<reg> >(string("IN") + (char)reg)
        {}
        format repr() const {
            return format("%1% += 1") % reg;
        }
    };
    typedef Increase<Reg::Index::X> Inx;
    typedef Increase<Reg::Index::Y> Iny;

    struct Dec : Op<Dec> {
        Dec() : Op<Dec>("DEC")
        {}
        format repr(const string& m) const {
            return format("%1% -= 1") % m;
        }
    };
    template<Reg::Index::All reg> struct Decrease : Op<Decrease<reg> > {
        // well, that's nasty also.
        Decrease() : Op<Decrease<reg> >(string("DE") + (char)reg)
        {}
        format repr() const {
            return format("%1% += 1") % (char)reg;
        }
    };
    typedef Decrease<Reg::Index::X> Dex;
    typedef Decrease<Reg::Index::Y> Dey;

    // logical/compare ops
    struct And : Op<And> {
        And() : Op<And>("AND") {}
        format repr(const string& m) const {
            return format("A &= %1%") % m;
        }
    };
    
    struct Ora : Op<Ora> {
        Ora() : Op<Ora>("ORA") {}
        format repr(const string& m) const {
            return format("A |= %1%") % m;
        }
    };
    
    struct Eor : Op<Eor> {
        Eor() : Op<Eor>("EOR") {}
        format repr(const string& m) const {
            return format("A ^= %1%") % m;
        }
    };
    
    struct Bit : Op<Bit> {
        Bit() : Op<Bit>("BIT") {}
        format repr(const string& m) const {
            return format("P.N,P.V = (A & %1%).(7,6)") % m;
        }
    };
    
    template<Reg::All reg> struct Compare : Op<Compare<reg> > {
        // well, that's nasty. and 'M' can not include 'A'
        Compare() : Op<Compare<reg> >(string("CMP") + (reg == 'M' ? 'P' : (char)reg))
        {}
        format repr(const string& m) const {
            return format("P.N,P.C,P.Z = %1% - %2%") % (char)reg % m;
        }
    };
    typedef Compare<Reg::M> Cmp;
    typedef Compare<Reg::X> Cpx;
    typedef Compare<Reg::Y> Cpy;
    
    // shift ops
    struct Asl : Op<Asl> {
        Asl() : Op("ASL") {}
        format repr(const string& m) const {
            return format("P.C = %1%.7; %1% = %1% << 1") % m;
        }
    };
    
    struct Lsr : Op<Lsr> {
        Lsr() : Op("LSR") {}
        format repr(const string& m) const {
            return format("P.C = %1%.0; %1% = %1% >> 1") % m;
        }
    };
    
    struct Rol : Op<Rol> {
        Rol() : Op("ROL") {}
        format repr(const string& m) const {
            return format("%1% = %1% << 1 | P.C") % m;
        }
    };
    
    struct Ror : Op<Ror> {
        Ror() : Op("ROR") {}
        format repr(const string& m) const {
            return format("%1% = (P.C << 7) | %1% >> 1") % m;
        }
    };
    
    namespace Flag {
        enum All {
            N = 'N',        // negative
            V = 'V',        // overflow
            B = 'B',        // break
            D = 'D',        // decimal, 2A03 does not use it, i guess
            I = 'I',        // interrupt
            Z = 'Z',        // zero
            C = 'C',        // carry
        };
    }
    
    // flag ops
    template<Flag::All flag> struct ClearFlag : Op<ClearFlag<flag> > {
        ClearFlag() : Op<ClearFlag<flag> >(string("CL") + (char)flag) {}
        format repr() const {
            return format("P.%1% = 0") % (char)flag;
        }
    };
    typedef ClearFlag<Flag::C> Clc;
    typedef ClearFlag<Flag::N> Cld;
    typedef ClearFlag<Flag::I> Cli;
    typedef ClearFlag<Flag::V> Clv;
    
    template<Flag::All flag> struct SetFlag : Op<SetFlag<flag> > {
        SetFlag() : Op<SetFlag<flag> >(string("SE") + (char)flag) {}
        format repr() const {
            return format("P.%1% = 1") % (char)flag;
        }
    };
    typedef SetFlag<Flag::C> Sec;
    typedef SetFlag<Flag::N> Sed;
    typedef SetFlag<Flag::I> Sei;
    
    // stack ops
    template<Reg::All reg> struct Push : Op<Push<reg> > {
        Push() : Op<Push<reg> >(string("PH") + (char)reg) {}
        format repr() const {
            return format("push8(%1%)") % (char)reg;
        }
    };
    typedef Push<Reg::A> Pha;
    typedef Push<Reg::P> Php;
    
    template<Reg::All reg> struct Pull : Op<Pull<reg> > {
        Pull() : Op<Pull<reg> >(string("PL") + (char)reg) {}
        format repr() const {
            return format("%1% = pop8()") % (char)reg;
        }
    };
    typedef Pull<Reg::A> Pla;
    typedef Pull<Reg::P> Plp;
    
    
    // branch ops
    struct Beq : Op<Beq> {
        Beq() : Op("BEQ") {}
        format repr(const string& m) const {
            return format("if (P.Z == 1) goto %1%") % m;
        }
    };
    struct Bmi : Op<Bmi> {
        Bmi() : Op<Bmi>("BMI") {}
        format repr(const string& m) const {
            return format("if (P.N == 1) goto %1%") % m;
        }
    };
    struct Bne : Op<Bne> {
        Bne() : Op("BNE") {}
        format repr(const string& m) const {
            return format("if (P.Z == 0) goto %1%") % m;
        }
    };
    struct Bpl : Op<Bpl> {
        Bpl() : Op("BPL") {}
        format repr(const string& m) const {
            return format("if (P.N == 0) goto %1%") % m;
        }
    };
    struct Bvc : Op<Bvc> {
        Bvc() : Op("BVC") {}
        format repr(const string& m) const {
            return format("if (P.V == 0) goto %1%") % m;
        }
    };
    struct Bvs : Op<Bvs> {
        Bvs() : Op<Bvs>("BVS") {}
        format repr(const string& m) const {
            return format("if (P.V == 1) goto %1%") % m;
        }
    };
    struct Bcc : Op<Bcc> {
        Bcc() : Op<Bcc>("BCC") {}
        format repr(const string& m) const {
            return format("if (P.C == 0) goto %1%") % m;
        }
    };
    struct Bcs : Op<Bcs> {
        Bcs() : Op<Bcs>("BCS") {}
        format repr(const string& m) const {
            return format("if (P.C == 1) goto %1%") % m;
        }
    };
    
    // load ops
    template<Reg::All reg> struct LoadReg : Op<LoadReg<reg> > {
        LoadReg() : Op<LoadReg<reg> >("LDA") {}
        format repr(const string& m) const {
            return format("%1% = %2%") % (char)reg % m;
        }
    };
    typedef LoadReg<Reg::A> Lda;
    typedef LoadReg<Reg::X> Ldx;
    typedef LoadReg<Reg::Y> Ldy;
    
    // store ops
    template<Reg::All from>
    struct Store : Op<Store<from> > {
        Store() : Op<Store<from> >(string("ST") + (char)from) {}
        format repr(const string& m) const {
            return format("%1% = %2%") % m % (char)from;
        }
    };
    typedef Store<Reg::A> Sta;
    typedef Store<Reg::X> Stx;
    typedef Store<Reg::Y> Sty;
    
    // transfer ops
    template<Reg::All from,
             Reg::All to>
    struct Transfer : Op<Transfer<from, to> > {
        Transfer() : Op<Transfer<from, to> >(string("T") + (char)from + (char)to) {}
        format repr() const {
            return format("%1% = %2%") % (char)from % (char)to;
        }
    };
    typedef Transfer<Reg::A, Reg::X> Tax;
    typedef Transfer<Reg::A, Reg::Y> Tay;
    typedef Transfer<Reg::S, Reg::X> Tsx;
    typedef Transfer<Reg::X, Reg::A> Txa;
    typedef Transfer<Reg::X, Reg::S> Txs;
    typedef Transfer<Reg::Y, Reg::A> Tya;
    
    // flow control ops
    struct Jmp : Op<Jmp> {
        Jmp() : Op<Jmp>("JMP") {}
        format repr(const string& m) const {
            return format("goto %1%") % m;
        }
    };
    
    struct Jsr : Op<Jsr> {
        Jsr() : Op<Jsr>("JSR") {}
        format repr(const string& m) const {
            return format("push16(pc+1); goto %1%;") % m;
        }
    };
    
    struct Rts : Op<Rts> {
        Rts() : Op<Rts>("RTS") {}
        format repr() const {
            return format("goto pop16() + 1");
        }
    };
    
    struct Rti : Op<Rti> {
        Rti() : Op<Rti>("RTI") {}
        format repr() const {
            return format("P = pop8(); goto pop16();");
        }
    };
    
    struct Brk : Op<Brk> {
        Brk() : Op<Brk>("BRK") {}
        format repr() const {
            return format("push16(pc+1); push8(P|0x10); P.i = 1; pc=$(IRQ_vector)");
        }
    };

    struct Nop : Op<Nop> {
        Nop() : Op<Nop>("NOP") {}
        format repr() const {
            return format("NOP");
        }
    };

    // it's a place holder for invalid instructions, vanilla nestopia
    // supports them, and lists them as "unofficial ops".
    struct Doh : Op<Doh> {
        Doh() : Op<Doh>("DOH") {}
        format repr() const {
            return format("DOH");
        }
    };
}

