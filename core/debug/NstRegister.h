//
//  NstRegister.h
//  OpenNestopia
//
//  Created by Kefu Chai on 09/09/12.
//
//

#pragma once

namespace Debug {
    // s/Reg/Addressing/
    namespace Reg {
        enum All {
            A = 'A',        // accumulator
            X = 'X',        // index
            Y = 'Y',        // index
            S = 'S',        // stack pointer, well not really ...
            M = 'M',        // ehh, A | zero page | zero page + x | absolute | absolute + x
            P = 'P',        // process status word
            SP = 'S',       // stack pointer
            PC = 'C',       // program counter
        };
        namespace Index {
            enum All {
                X = 'X',
                Y = 'Y',
            };
        }
    }

    enum AccessMode {
        NONE  = 0,
        READ  = 1 << 0,
        WRITE = 1 << 1,
        EXEC  = 1 << 2,
        RW    = READ|WRITE,
    };

    struct Access {
        uint16_t addr;
        AccessMode mode;
    };
}

