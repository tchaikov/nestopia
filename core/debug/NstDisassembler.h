//
//  NstDisassembler.h
//  OpenNestopia
//
//  Created by Kefu Chai on 09/09/12.
//
//

#ifndef __OpenNestopia__NstDisassembler__
#define __OpenNestopia__NstDisassembler__

#include <iostream>

namespace Nes {
    namespace Core {
        class Cpu;
    }
    namespace Debug {
        class Disassembler {
        private:
            static Opcode op0x00(); static Opcode op0x01(); static Opcode op0x02(); static Opcode op0x03();
			static Opcode op0x04(); static Opcode op0x05(); static Opcode op0x06(); static Opcode op0x07();
			static Opcode op0x08(); static Opcode op0x09(); static Opcode op0x0A(); static Opcode op0x0B();
			static Opcode op0x0C(); static Opcode op0x0D(); static Opcode op0x0E(); static Opcode op0x0F();
			static Opcode op0x10(); static Opcode op0x11(); static Opcode op0x12(); static Opcode op0x13();
			static Opcode op0x14(); static Opcode op0x15(); static Opcode op0x16(); static Opcode op0x17();
			static Opcode op0x18(); static Opcode op0x19(); static Opcode op0x1A(); static Opcode op0x1B();
			static Opcode op0x1C(); static Opcode op0x1D(); static Opcode op0x1E(); static Opcode op0x1F();
			static Opcode op0x20(); static Opcode op0x21(); static Opcode op0x22(); static Opcode op0x23();
			static Opcode op0x24(); static Opcode op0x25(); static Opcode op0x26(); static Opcode op0x27();
			static Opcode op0x28(); static Opcode op0x29(); static Opcode op0x2A(); static Opcode op0x2B();
			static Opcode op0x2C(); static Opcode op0x2D(); static Opcode op0x2E(); static Opcode op0x2F();
			static Opcode op0x30(); static Opcode op0x31(); static Opcode op0x32(); static Opcode op0x33();
			static Opcode op0x34(); static Opcode op0x35(); static Opcode op0x36(); static Opcode op0x37();
			static Opcode op0x38(); static Opcode op0x39(); static Opcode op0x3A(); static Opcode op0x3B();
			static Opcode op0x3C(); static Opcode op0x3D(); static Opcode op0x3E(); static Opcode op0x3F();
			static Opcode op0x40(); static Opcode op0x41(); static Opcode op0x42(); static Opcode op0x43();
			static Opcode op0x44(); static Opcode op0x45(); static Opcode op0x46(); static Opcode op0x47();
			static Opcode op0x48(); static Opcode op0x49(); static Opcode op0x4A(); static Opcode op0x4B();
			static Opcode op0x4C(); static Opcode op0x4D(); static Opcode op0x4E(); static Opcode op0x4F();
			static Opcode op0x50(); static Opcode op0x51(); static Opcode op0x52(); static Opcode op0x53();
			static Opcode op0x54(); static Opcode op0x55(); static Opcode op0x56(); static Opcode op0x57();
			static Opcode op0x58(); static Opcode op0x59(); static Opcode op0x5A(); static Opcode op0x5B();
			static Opcode op0x5C(); static Opcode op0x5D(); static Opcode op0x5E(); static Opcode op0x5F();
			static Opcode op0x60(); static Opcode op0x61(); static Opcode op0x62(); static Opcode op0x63();
			static Opcode op0x64(); static Opcode op0x65(); static Opcode op0x66(); static Opcode op0x67();
			static Opcode op0x68(); static Opcode op0x69(); static Opcode op0x6A(); static Opcode op0x6B();
			static Opcode op0x6C(); static Opcode op0x6D(); static Opcode op0x6E(); static Opcode op0x6F();
			static Opcode op0x70(); static Opcode op0x71(); static Opcode op0x72(); static Opcode op0x73();
			static Opcode op0x74(); static Opcode op0x75(); static Opcode op0x76(); static Opcode op0x77();
			static Opcode op0x78(); static Opcode op0x79(); static Opcode op0x7A(); static Opcode op0x7B();
			static Opcode op0x7C(); static Opcode op0x7D(); static Opcode op0x7E(); static Opcode op0x7F();
			static Opcode op0x80(); static Opcode op0x81(); static Opcode op0x82(); static Opcode op0x83();
			static Opcode op0x84(); static Opcode op0x85(); static Opcode op0x86(); static Opcode op0x87();
			static Opcode op0x88(); static Opcode op0x89(); static Opcode op0x8A(); static Opcode op0x8B();
			static Opcode op0x8C(); static Opcode op0x8D(); static Opcode op0x8E(); static Opcode op0x8F();
			static Opcode op0x90(); static Opcode op0x91(); static Opcode op0x92(); static Opcode op0x93();
			static Opcode op0x94(); static Opcode op0x95(); static Opcode op0x96(); static Opcode op0x97();
			static Opcode op0x98();
        };
    }
}

#endif /* defined(__OpenNestopia__NstDisassembler__) */
