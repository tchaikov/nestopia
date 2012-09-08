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
        using Nes::Core::Cpu;

        struct Operand {
            Operand(const Core::Cpu& cpu, const boost::format& fmt)
            : cpu_(cpu), fmt_(fmt) {}
            Operand(const Core::Cpu& cpu, const char* fmt)
            : cpu_(cpu), fmt_(fmt) {}
            std::string str(uint pc) {
                return (fmt_ % operand(pc)).str();
            }
            virtual uint operand(uint& pc) const = 0;
        protected:
            uint fetchPc8(uint &pc) const {
                const uint address = cpu_.map.Peek16(pc);
                ++pc;
                return address;
            }
            uint fetchPc16(uint &pc) {
                const uint address = cpu_.map.Peek16(pc);
                pc += 2;
                return address;
            }
        private:
            const Cpu& cpu_;
            boost::format fmt_;
        };

        // immediate addressing
        struct Imm : public Operand {
            // should be LDA #10 ; load 10 ($0A) into the accumulator
            // but i prefer decimal the presentation
            Imm(const Cpu& cpu) : Operand(cpu, "#%02X") {}
            uint operand(uint& pc) const {
                return fetchPc8(pc);
            }
        };

        // absolute addressing
        struct Abs : public Operand {
            Abs(const Cpu& cpu) : Operand(cpu, "%04X") {}
            uint operand(uint& pc) {
                return fetchPc16(pc);
            }
        };

        // Zero page addressing
        struct Zpg : public Operand {
            Zpg(const Cpu& cpu) : Operand(cpu, "%02X") {}
            uint operand(uint &pc) {
                return fetchPc16(pc);
            }
        };

        // Zero page indexed addressing (X or Y)
        struct ZpgReg : public Operand {
            // LDA $3F, X
            ZpgReg(const Cpu& cpu, char reg)
            : Operand(cpu, boost::format("$%2$02X, %1$C") % reg) {}
            uint operand(uint &pc) {
                return fetchPc8(pc);
            }
        };

        // Absolute indexed addressing
        struct AbsReg : public Operand {
            // LDA $8000, X
            AbsReg(const Cpu& cpu, char reg)
            : Operand(cpu, boost::format("$%2$04X, %1$C") % reg) {}
            uint operand(uint &pc) {
                return fetchPc16(pc);
            }
        };

        // Indexed indirect addressing
        struct IndReg : public Operand {
            IndReg(const Cpu& cpu, char reg)
            : Operand(cpu, boost::format("($%2$02X, %1$C)") % reg) {}
            uint operand(uint &pc) {
                return fetchPc8(pc);
            }
        };

        struct Instruction {
            const std::string instruction;
            boost::scoped_ptr<Operand> operand;
            uint8_t opcode;
            std::string desc;
            static Disassembler::IR(const char *inst_,
                                    uint operand_,
                                    const char* fmt);
        };
    }
}
