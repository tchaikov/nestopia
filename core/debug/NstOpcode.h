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
    using boost::format;
    using std::string;

    struct Decoded {
        string str;
        string repr;
    };
    class Opcode {
    public:
        virtual ~Opcode() {}
        virtual Decoded decode(uint& pc) = 0;
        virtual bool triggers(const Condition& cond) {
            return false;
        }
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
          op_(Operateur::get()),
          addr_(Addressing::get())
        {}
        virtual Decoded decode(uint& pc) {
            uint m = addr_.fetch(pc);
            Decoded decoded;
            decoded.str = (fmt_ % op_.str() % addr_.str(m)).str();
            decoded.repr = (op_.repr(addr_.repr(pc, m))).str();
            return decoded;                    
        }
        virtual bool triggers(const Condition& cond) {
            return false;
        }
    private:
        format fmt_;
        const Operateur& op_;
        const Addressing& addr_;
    };

    template <class Operateur,
              AccessMode mode>
    struct Opcode_<Operateur, Implied, mode> : public Opcode {
    public:
        Opcode_()
        : op_(Operateur::get())
        {}
        virtual Decoded decode(uint& pc) {
            // we are in implied addressing mode, so no need to fetch the
            // operand.
            Decoded decoded;
            decoded.str = op_.str();
            decoded.repr = op_.repr().str();
            return decoded;
        }
    private:
        const Operateur& op_;
    };
}

