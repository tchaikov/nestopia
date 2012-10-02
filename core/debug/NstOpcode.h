//
//  NstOpcode.h
//  OpenNestopia
//
//  Created by Kefu Chai on 08/09/12.
//
//

#pragma once

#include <string>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/format.hpp>

#include "../NstBase.hpp"
#include "../NstCpu.hpp"

#include "NstAddressing.h"
#include "NstBreakpoint.h"

namespace Nes {
    namespace Core {
        class Cpu;
    }
}

namespace Debug {
    using std::string;

    struct Decoded {
        string str;
        string repr;
    };
    class Opcode {
    public:
        virtual ~Opcode() {}
        virtual Decoded decode(uint16_t& pc) const = 0;
        virtual Access access(uint16_t pc) const = 0;
        virtual int flow_control_type() const = 0;
    };
    // opcode is a combination of an operator and its operand
    // any opcode which has an operand goes here, even it's an implicit
    // operand.
    template<class Operateur,
             class Addressing,
             AccessMode mode>
    class Opcode_ : public Opcode {
    public:
        Opcode_()
        : fmt_("%1% %2%"),
          op_(Operateur::instance()),
          addr_(Addressing::instance())
        {}
        virtual Decoded decode(uint16_t& pc) const {
            uint m = addr_.fetch(pc);
            Decoded decoded;
            decoded.str = (boost::format(fmt_) % op_.str() % addr_.str(m)).str();
            decoded.repr = op_.repr(addr_.repr(pc, m)).str();
            return decoded;                    
        }
        virtual Access access(uint16_t pc) const {
            return {addr_.get(addr_.fetch(pc)), mode};
        }
        virtual int flow_control_type() const {
            return Operateur::flow_control_type;
        }
    private:
        const std::string fmt_;
        const Operateur& op_;
        const Addressing& addr_;
    };

    template <class Operateur,
              AccessMode mode>
    struct Opcode_<Operateur, Acc, mode> : public Opcode {
    public:
        Opcode_()
        : op_(Operateur::instance())
        {}
        virtual Decoded decode(uint16_t& pc) const {
            // we are in Acc addressing mode, so no need to fetch the
            // operand.
            Decoded decoded;
            decoded.str = (boost::format("%1% A") % op_.str()).str();
            decoded.repr = op_.repr("A").str();
            return decoded;
        }
        // no memory space is touched in Acc addressing mode
        virtual Access access(uint16_t pc) const {
            return {0, NONE};
        }
        virtual int flow_control_type() const {
            return Operateur::flow_control_type;
        }
    private:
        const Operateur& op_;
    };


    template <class Operateur,
              AccessMode mode>
    struct Opcode_<Operateur, Implied, mode> : public Opcode {
    public:
        Opcode_()
        : op_(Operateur::instance())
        {}
        virtual Decoded decode(uint16_t& pc) const {
            // we are in implied addressing mode, so no need to fetch the
            // operand.
            Decoded decoded;
            decoded.str = op_.str();
            decoded.repr = op_.repr().str();
            return decoded;
        }
        // TODO: the memory invoved in implied addressing can be a little bit
        //       complicated than the others.
        virtual Access access(uint16_t pc) const {
            return {0, NONE};
        }
        virtual int flow_control_type() const {
            return Operateur::flow_control_type;
        }
    private:
        const Operateur& op_;
    };
}

