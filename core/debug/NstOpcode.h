//
//  NstDisassembler.h
//  OpenNestopia
//
//  Created by Kefu Chai on 08/09/12.
//
//

#pragma once

#include <string>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/format.hpp>

#include "NstBase.hpp"
#include "NstCpu.hpp"

namespace Nes {
    namespace Core {
        class Cpu;
    }
    namespace Disassembler {
        using boost::format;
        using std::string;

        struct Operand {
            Operand(const format& fmt)
            : fmt_(fmt) {}
            Operand(const char* fmt)
            : fmt_(fmt) {}
            string str(uint data) {
                return (fmt_ % data).str();
            }
            format repr(uint pc, uint data) {
                return fmt_ % data;
            }

        protected:
            uint fetchPc8(uint &pc) const {
                const uint address = cpu_->map.Peek16(pc);
                ++pc;
                return address;
            }
            uint fetchPc16(uint &pc) {
                const uint address = cpu_->map.Peek16(pc);
                pc += 2;
                return address;
            }
        private:
            const Nes::Core::Cpu* cpu_;
            format fmt_;
        };

        // Implied addressing
        struct Imp {  
        };
        // Accumulator addressing
        struct Acc : Operand {
            Acc() : Operand("%A") {}
            uint fetch(uint&) const {
                // never reach here
                BOOST_ASSERT(false);
            }
        };

        // immediate addressing
        struct Imm : Operand {
            // should be LDA #10 ; load 10 ($0A) into the accumulator
            // but i prefer the hex presentation
            Imm() : Operand("#$%02X") {}
            uint fetch(uint& pc) const {
                return fetchPc8(pc);
            }
        };

        // absolute addressing
        struct Abs : Operand {
            Abs() : Operand("$%04X") {}
            uint fetch(uint& pc) {
                return fetchPc16(pc);
            }
        };

        // Zero page addressing
        struct Zpg : Operand {
            Zpg() : Operand("$%02X") {}
            uint fetch(uint &pc) {
                return fetchPc16(pc);
            }
        };

        // s/Reg/Addressing/
        namespace Reg {
            enum All {
                A = 'A',        // accumulator
                X = 'X',        // index
                Y = 'Y',        // index
                S = 'S',        // stack pointer, well not really ...
                M = 'M',        // ehh, A | zero page | zero page + x | absolute | absolute + x
                P = 'P',        // process status word
            };
            namespace Index {
                enum All {
                    X = 'X',
                    Y = 'Y',
                };
            }
        }

        // Zero page indexed addressing (X or Y)
        template<Reg::Index::All reg> struct ZpgReg : Operand {
            // LDA $3F, X
            ZpgReg()
            : Operand(format("$2%2$02X, %1$C") % reg) {}
            uint fetch(uint &pc) {
                return fetchPc8(pc);
            }
        };
        typedef ZpgReg<Reg::Index::X> ZpgX;
        typedef ZpgReg<Reg::Index::Y> ZpgY;

        // Absolute indexed addressing
        template<Reg::Index::All reg> struct AbsReg : Operand {
            // LDA $8000, X
            AbsReg()
            : Operand(format("$2%2$04X, %1$C") % reg) {}
            uint fetch(uint &pc) {
                return fetchPc16(pc);
            }
        };
        typedef AbsReg<Reg::Index::X> AbsX;
        typedef AbsReg<Reg::Index::Y> AbsY;

        // Indexed indirect addressing
        template<Reg::Index::All reg> struct IndReg : Operand {
            IndReg()
            : Operand(format("($2%2$02X, %1$C)") % reg) {}
            uint fetch(uint &pc) {
                return fetchPc8(pc);
            }
        };
        typedef IndReg<Reg::Index::X> IndX;
        typedef IndReg<Reg::Index::Y> IndY;

        // Relative addressing
        struct Rel : Operand {
            Rel() : Operand("$+02X") {}
            uint fetch(uint& pc) {
                return fetchPc8(pc);
            }
            format repr(uint pc, uint m) {
                return format("0x$04X") % (pc + m);
            }
        };

        struct Operator {
            Operator(const string& op)
                : op_(op) {}
            Operator(const char* op)
                : op_(op) {}
            /// @c LDA for example
            const string& str() {
                return op_;
            }
        private:
            const string op_;
        };

        // N2A03 has no BCD mode, so we don't care the decimal flag
        // arithmetic ops
        struct Adc : Operator {
            Adc() : Operator("ADC") {}
            format repr(const string& m) const {
                return format("A += %1% + P.C") % m;
            }
        };

        struct Sbc : Operator {
            Sbc() : Operator("SBC") {}
            format repr(const string& m) const {
                return format("A -= %1% - !P.C") % m;
            }
        };

        // inc and dec ops
        template<Reg::All reg> struct Increase : Operator {
            // well, that's nasty too.
            Increase() : Operator(string("IN") + (reg == 'M' ? 'C' : (char)reg))
            {}
            format repr(const string& m) const {
                return format("%1% += 1") % (char)reg;
            }
        };
        // M does not include A here
        typedef Increase<Reg::M> Inc;
        typedef Increase<Reg::X> Inx;
        typedef Increase<Reg::Y> Iny;
                
        template<Reg::All reg> struct Decrease : Operator {
            // well, that's nasty also.
            Decrease() : Operator(string("DE") + (reg == 'M' ? 'C' : (char)reg))
            {}
            format repr(const string& m) const {
                return format("%1% += 1") % (char)reg;
            }
        };
        typedef Increase<Reg::M> Inc;
        typedef Increase<Reg::X> Inx;
        typedef Increase<Reg::Y> Iny;

        // logical/compare ops
        struct And : Operator {
            And() : Operator("AND") {}
            format repr(const string& m) const {
                return format("A &= %1%") % m;
            }
        };

        struct Ora : Operator {
            Ora() : Operator("ORA") {}
            format repr(const string& m) const {
                return format("A |= %1%") % m;
            }
        };

        struct Eor : Operator {
            Eor() : Operator("EOR") {}
            format repr(const string& m) const {
                return format("A ^= %1%") % m;
            }
        };

        struct Bit : Operator {
            Bit() : Operator("BIT") {}
            format repr(const string& m) const {
                return format("P.N,P.V = (A & %1%).(7,6)") % m;
            }
        };

        template<Reg::All reg> struct Compare : Operator {
            // well, that's nasty. and 'M' can not include 'A'
            Compare() : Operator(string("CMP") + (reg == 'M' ? 'P' : (char)reg))
            {}
            format repr(const string& m) const {
                return format("P.N,P.C,P.Z = %1% - %2%") % (char)reg % m;
            }
        };
        typedef Compare<Reg::M> Cmp;
        typedef Compare<Reg::X> Cpx;
        typedef Compare<Reg::Y> Cpy;

        // shift ops
        struct Asl : Operator {
            Asl() : Operator("ASL") {}
            format repr(const string& m) const {
                return format("P.C = %1%.7; %1% = %1% << 1") % m;
            }
        };
        
        struct Lsr : Operator {
            Lsr() : Operator("LSR") {}
            format repr(const string& m) const {
                return format("P.C = %1%.0; %1% = %1% >> 1") % m;
            }
        };

        struct Rol : Operator {
            Rol() : Operator("ROL") {}
            format repr(const string& m) const {
                return format("%1% = %1% << 1 | P.C") % m;
            }
        };

        struct Ror : Operator {
            Ror() : Operator("ROR") {}
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
        template<Flag::All flag> struct ClearFlag : Operator {
            ClearFlag() : Operator(string("CL") + (char)flag) {}
            format repr() const {
                return format("P.%1% = 0") % (char)flag;
            }
        };
        typedef ClearFlag<Flag::C> Clc;
        typedef ClearFlag<Flag::N> Cld;
        typedef ClearFlag<Flag::I> Cli;
        typedef ClearFlag<Flag::V> Clv;

        template<Flag::All flag> struct SetFlag : Operator {
            SetFlag() : Operator(string("SE") + (char)flag) {}
            format repr() const {
                return format("P.%1% = 1") % (char)flag;
            }
        };
        typedef SetFlag<Flag::C> Sec;
        typedef SetFlag<Flag::N> Sed;
        typedef SetFlag<Flag::I> Sei;

        // stack ops
        template<Reg::All reg> struct Push : Operator {
            Push() : Operator(string("PH") + (char)reg) {}
            format repr() const {
                return format("push8(%1%)") % (char)reg;
            }
        };
        typedef Push<Reg::A> Pha;
        typedef Push<Reg::P> Php;

        template<Reg::All reg> struct Pull : Operator {
            Pull() : Operator(string("PL") + (char)reg) {}
            format repr() const {
                return format("%1% = pop8()") % (char)reg;
            }
        };
        typedef Pull<Reg::A> Pla;
        typedef Pull<Reg::P> Plp;


        // branch ops
        struct Beq : Operator {
            Beq() : Operator("BEQ") {}
            format repr(const string& m) const {
                return format("if (P.Z == 1) goto %1%") % m;
            }
        };
        struct Bmi : Operator {
            Bmi() : Operator("BMI") {}
            format repr(const string& m) const {
                return format("if (P.N == 1) goto %1%") % m;
            }
        };
        struct Bne : Operator {
            Bne() : Operator("BNE") {}
            format repr(const string& m) const {
                return format("if (P.Z == 0) goto %1%") % m;
            }
        };
        struct Bpl : Operator {
            Bpl() : Operator("BPL") {}
            format repr(const string& m) const {
                return format("if (P.N == 0) goto %1%") % m;
            }
        };
        struct Bvc : Operator {
            Bvc() : Operator("BVC") {}
            format repr(const string& m) const {
                return format("if (P.V == 0) goto %1%") % m;
            }
        };
        struct Bvs : Operator {
            Bvs() : Operator("BVS") {}
            format repr(const string& m) const {
                return format("if (P.V == 1) goto %1%") % m;
            }
        };
        struct Bcc : Operator {
            Bcc() : Operator("BCC") {}
            format repr(const string& m) const {
                return format("if (P.C == 0) goto %1%") % m;
            }
        };
        struct Bcs : Operator {
            Bcs() : Operator("BCS") {}
            format repr(const string& m) const {
                return format("if (P.C == 1) goto %1%") % m;
            }
        };

        // load ops
        template<Reg::All reg> struct LoadReg : Operator {
            LoadReg() : Operator("LDA") {}
            format repr(const string& m) const {
                return format("%1%,Z,-,N = %2%") % reg % m;
            }
        };
        typedef LoadReg<Reg::A> Lda;
        typedef LoadReg<Reg::X> Ldx;
        typedef LoadReg<Reg::Y> Ldy;

        // store ops
        template<Reg::All from>
        struct Store : Operator {
            Store() : Operator(string("ST") + (char)from) {}
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
        struct Transfer : Operator {
            Transfer() : Operator(string("T") + (char)from + (char)to) {}
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
        struct Jmp : Operator {
            Jmp() : Operator("JMP") {}
            format repr(const string& m) const {
                return format("goto %1%") % m;
            }
        };

        struct Jsr : Operator {
            Jsr() : Operator("JSR") {}
            format repr(const string& m) const {
                return format("push16(pc+1); goto %1%;") % m;
            }
        };

        struct Rts : Operator {
            Rts() : Operator("RTS") {}
            format repr(const string& m) const {
                return format("goto pop16() + 1");
            }
        };

        struct Rti : Operator {
            Rti() : Operator("RTI") {}
            format repr(const string& m) const {
                return format("P = pop(8); goto pop16();");
            }
        };

        struct Brk : Operator {
            Brk() : Operator("BRK") {}
            format repr(const string& m) const {
                return format("push16(pc+1); push8(P|0x10); P.i = 1; pc=$(IRQ_vector)");
            }
        };

        struct Decoded {
            string str;
            string repr;
        };

        // opcode is a combination of operator and its operand
        // any opcode which has an operand goes here, even it's an implicit
        // operand.
        template<class Operateur,
                 class Addressing>
        struct Opcode {
            // TODO: can use some singleton of operator and operand here ?
            Opcode()
             : fmt_("%1% %2%")
            {}
            Decoded decode(uint& pc) {
                uint m = addr_.fetch(pc);
                Decoded decoded;
                decoded.str = fmt_ % op_.str() % addr_.str(m);
                decoded.repr = op_.repr(addr_.repr(m));
                return decoded;
                    
            }
        private:
            Operateur& op_;
            Addressing& addr_;
            format fmt_;
        };
        template <class Operateur>
        struct Opcode<Operator, Imp> {
            Decoded decode(uint& pc) {
                // we are in implied addressing mode, so no need to fetch the
                // operand.
                Decoded decoded;
                decoded.str = op_.str();
                decoded.repr = op_.repr();
                return decoded;
            }
        };
        
        // any opcode which does not has any operand falls into this class.
        // typically, all branch ops are of this class.
        template<class Operator>
        struct Opcode {
            Decoded decode(uint& pc) {
                Decoded decoded;
                decoded.str = 
            Decoded disassemble(uint pc);

        private:
            static Opcode op0x00(); static Opcode op0x01(); static Opcode op0x02(); static Opcode op0x03();
			static Opcode op0x04(); static Opcode op0x05(); static Opcode op0x06(); static Opcode op0x07();
			static Opcode op0x08(); static Opcode op0x09(); static Opcode op0x0A(); static Opcode op0x0B();
			static Opcode op0x0C(); static Opcode op0x0D(); static Opcode op0x0E(); static Opcode op0x0F();
			static Opcode op0x10(); static Opcode op0x11(); static Opcode op0x12(); static Opcode op0x13();
			static Opcode op0x14(); static Opcode op0x15(); static Opcode op0x16(); static Opcode op0x17();
			static Opcode op0x18(); static Opcode op0x19(); static Opcode op0x1A(); static Opcode op0x1B();
			static Opcode op0x1C(); static Opcode op0x1D(); static Opcode op0x1E(); static Opcode op0x1F();
			static Opcode op0x20(); static Opcode op0x21(); static Opcode op0x22(); static Opcode op0x23();
			static Opcode op0x24(); static Opcode op0x25(); static Opcode op0x26(); static Opcode op0x27();
			static Opcode op0x28(); static Opcode op0x29(); static Opcode op0x2A(); static Opcode op0x2B();
			static Opcode op0x2C(); static Opcode op0x2D(); static Opcode op0x2E(); static Opcode op0x2F();
			static Opcode op0x30(); static Opcode op0x31(); static Opcode op0x32(); static Opcode op0x33();
			static Opcode op0x34(); static Opcode op0x35(); static Opcode op0x36(); static Opcode op0x37();
			static Opcode op0x38(); static Opcode op0x39(); static Opcode op0x3A(); static Opcode op0x3B();
			static Opcode op0x3C(); static Opcode op0x3D(); static Opcode op0x3E(); static Opcode op0x3F();
			static Opcode op0x40(); static Opcode op0x41(); static Opcode op0x42(); static Opcode op0x43();
			static Opcode op0x44(); static Opcode op0x45(); static Opcode op0x46(); static Opcode op0x47();
			static Opcode op0x48(); static Opcode op0x49(); static Opcode op0x4A(); static Opcode op0x4B();
			static Opcode op0x4C(); static Opcode op0x4D(); static Opcode op0x4E(); static Opcode op0x4F();
			static Opcode op0x50(); static Opcode op0x51(); static Opcode op0x52(); static Opcode op0x53();
			static Opcode op0x54(); static Opcode op0x55(); static Opcode op0x56(); static Opcode op0x57();
			static Opcode op0x58(); static Opcode op0x59(); static Opcode op0x5A(); static Opcode op0x5B();
			static Opcode op0x5C(); static Opcode op0x5D(); static Opcode op0x5E(); static Opcode op0x5F();
			static Opcode op0x60(); static Opcode op0x61(); static Opcode op0x62(); static Opcode op0x63();
			static Opcode op0x64(); static Opcode op0x65(); static Opcode op0x66(); static Opcode op0x67();
			static Opcode op0x68(); static Opcode op0x69(); static Opcode op0x6A(); static Opcode op0x6B();
			static Opcode op0x6C(); static Opcode op0x6D(); static Opcode op0x6E(); static Opcode op0x6F();
			static Opcode op0x70(); static Opcode op0x71(); static Opcode op0x72(); static Opcode op0x73();
			static Opcode op0x74(); static Opcode op0x75(); static Opcode op0x76(); static Opcode op0x77();
			static Opcode op0x78(); static Opcode op0x79(); static Opcode op0x7A(); static Opcode op0x7B();
			static Opcode op0x7C(); static Opcode op0x7D(); static Opcode op0x7E(); static Opcode op0x7F();
			static Opcode op0x80(); static Opcode op0x81(); static Opcode op0x82(); static Opcode op0x83();
			static Opcode op0x84(); static Opcode op0x85(); static Opcode op0x86(); static Opcode op0x87();
			static Opcode op0x88(); static Opcode op0x89(); static Opcode op0x8A(); static Opcode op0x8B();
			static Opcode op0x8C(); static Opcode op0x8D(); static Opcode op0x8E(); static Opcode op0x8F();
			static Opcode op0x90(); static Opcode op0x91(); static Opcode op0x92(); static Opcode op0x93();
			static Opcode op0x94(); static Opcode op0x95(); static Opcode op0x96(); static Opcode op0x97();
			static Opcode op0x98();
        };
    }
}
