//
//  NstDisassembler.cpp
//  OpenNestopia
//
//  Created by Kefu Chai on 08/09/12.
//
//

#include "NstDisassembler.h"


inline std::string Cpu::Disassemble(uint where) const
{
    const uint opcode = map.Peek8(where++);
}

