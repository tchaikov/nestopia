//
//  DebuggerDelegateBridge.h
//  OpenNestopia
//
//  Created by Kefu Chai on 18/09/12.
//
//

#pragma once

#include <string>

namespace Debug {

    struct DelegateImpl;

    // call backs called by Debug::Debugger
    class DelegateBridge {
    public:
        DelegateBridge(void *delegate);
        ~DelegateBridge();
        void print_console(const std::string& msg);
        void will_step_to(uint16_t pc);
        void will_jump_to(uint16_t pc);
        void will_trigger_breakpoint(uint16_t pc, int breakpoint);
        void update_sprite(int x, int y);

    private:
        DelegateImpl* impl;
    };
}