//
//  NstCondition.h
//  OpenNestopia
//
//  Created by Kefu Chai on 08/09/12.
//
//

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include "NstRegister.h"

namespace Debug {

    struct Breakpoint {
        uint16_t address;
        AccessMode access;
        bool enabled;
    };

    class BreakpointManager {
    public:
        int set(uint16_t pc, AccessMode access);
        bool remove(int index);
        bool enable(int index);
        bool disable(int index);
        const Breakpoint* lookup(int index) const;

        int test_access(const Access& access);
    private:

        // <index, breakpoint> pair, will always use the smallest available
        // integer for the newly added breakpoint.
        typedef std::map<int, Breakpoint> Breakpoints;
        Breakpoints breakpoints_;
    };
}

