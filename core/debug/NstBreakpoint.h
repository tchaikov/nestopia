//
//  NstCondition.h
//  OpenNestopia
//
//  Created by Kefu Chai on 08/09/12.
//
//

#pragma once

#include <boost/shared_ptr.hpp>

namespace Debug {
    struct Condition;

    struct Breakpoint {
        uint16_t address;
        bool enabled;
        boost::shared_ptr<Condition> condition;
        Breakpoint(uint16_t addr)
        : address(addr),
          enabled(true)
        {}
    };

    struct Condition {
        uint16_t accessed_address;
        AccessMode access_mode;
        Condition(uint16_t address, AccessMode mode)
        : accessed_address(address),
          access_mode(mode)
        {}
    };
}

