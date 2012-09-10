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
    }
}
