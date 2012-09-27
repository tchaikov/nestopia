//
//  NstAddressing.cpp
//  OpenNestopia
//
//  Created by Kefu Chai on 09/09/12.
//
//

#include "NstAddressing.h"

namespace Debug {
    namespace detail {
        static const ::Nes::Core::Cpu *cpu_ = nullptr;
    }
    void set_cpu(const ::Nes::Core::Cpu *cpu) {
        detail::cpu_ = cpu;
    }
    const Cpu* get_cpu() {
        return detail::cpu_;
    }
}
