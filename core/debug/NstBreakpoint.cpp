//
//  NstBreakpoint.cpp
//  OpenNestopia
//
//  Created by Kefu Chai on 18/09/12.
//
//

#include "NstBreakpoint.h"

namespace Debug {
    int
    BreakpointManager::set(uint16_t pc, AccessMode access)
    {
        // look for the first usable index
        int index = 0;
        while (breakpoints_.find(index++) == breakpoints_.end());
        Breakpoint bp = {pc, access, true};
        breakpoints_.insert(std::make_pair(index, bp));
        return index;
    }

    void
    BreakpointManager::remove(int index)
    {
        breakpoints_.erase(index);
    }
    
    void
    BreakpointManager::disable(int index)
    {
        auto found = breakpoints_.find(index);
        if (found == breakpoints_.end())
            return;
        found->second.enabled = false;
    }

    void
    BreakpointManager::enable(int index)
    {
        auto found = breakpoints_.find(index);
        if (found == breakpoints_.end())
            return;
        found->second.enabled = true;
    }

    const Breakpoint*
    BreakpointManager::lookup(int index) const
    {
        auto found = breakpoints_.find(index);
        if (found == breakpoints_.end())
            return nullptr;
        return &found->second;
    }

    int
    BreakpointManager::test_access(const Access& access)
    {
        for (auto it : breakpoints_) {
            Breakpoint& bp = it.second;
            if (bp.address == access.addr &&
                bp.access | access.mode)
                return it.first;
        }
        return -1;
    }
}