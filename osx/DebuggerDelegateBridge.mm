//
//  DebuggerDelegateBridge.cpp
//  OpenNestopia
//
//  Created by Kefu Chai on 18/09/12.
//
//

#include "DebuggerDelegateBridge.h"
#import "DebuggerDelegate.h"
#import <Foundation/Foundation.h>

namespace Debug {

    struct DelegateImpl {
        id<DebuggerDelegate> delegate;
    };

    DelegateBridge::DelegateBridge(void *delegate)
    : impl(new DelegateImpl)
    {
        impl->delegate = (__bridge id<DebuggerDelegate>)delegate;
    }

    DelegateBridge::~DelegateBridge()
    {
        impl->delegate = nil;
        delete impl;
    }

    void
    DelegateBridge::print_console(const std::string& msg)
    {
        [impl->delegate printConsole:[NSString stringWithUTF8String:msg.c_str()]];
    }

    void
    DelegateBridge::will_step_to(uint16_t pc)
    {
        [impl->delegate willStepToAddress:pc];
    }

    void
    DelegateBridge::will_jump_to(uint16_t pc)
    {
        /// @todo
    }

    void
    DelegateBridge::will_trigger_breakpoint(uint16_t pc, int breakpoint)
    {
        [impl->delegate breakpoint:breakpoint triggeredAt:pc];
    }

    void
    DelegateBridge::update_sprite(int x, int y)
    {
        // TODO: re-read the sprite, and update the sprit window
    }
}