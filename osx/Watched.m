//
//  Watched.m
//  OpenNestopia
//
//  Created by Kefu Chai on 01/10/12.
//
//

#import "Watched.h"
#import "DebuggerBridge.h"

@implementation Watched

+ (Watched *)watchedWithName:(NSString *)name {
    if ([Watched isAddress:name]) {
        return [[Watched alloc] initAsMem:name];
    } else if ([Watched isRegister:name]) {
        return [[Watched alloc] initAsReg:name];
    }
    return nil;
}

- (BOOL)update:(DebuggerBridge *)debugger {
    NSString *oldValue = self.value;
    self.value = _getValue(debugger);
    return ![self.value isEqualToString:oldValue];
}

+ (BOOL)isAddress:(NSString *)name {
    return [name hasPrefix:@"$"];
}

+ (BOOL)isRegister:(NSString *)name {
    return ([name hasPrefix:@"%"]);
}

+ (Reg)nameToReg:(NSString *)name {
    name = [name uppercaseString];
    if ([name isEqualToString:@"%A"]) {
        return A;
    } else if ([name isEqualToString:@"%X"]) {
        return X;
    } else if ([name isEqualToString:@"%Y"]) {
        return Y;
    } else if ([name isEqualToString:@"%PC"]) {
        return PC;
    } else if ([name isEqualToString:@"%SP"]) {
        return SP;
    } else if ([name isEqualToString:@"%P"]) {
        return P;
    } else {
        return UNKNOWN;
    }
}

- (id)initAsMem:(NSString *)addrString {
    if (self = [super init]) {
        self.format = @"%d";
        _getValue = ^ NSString *(DebuggerBridge *debugger) {
            /// TODO: support more addressing modes here
            uint16_t address = [[addrString substringFromIndex:1] intValue];
            uint8_t value = [debugger peek8:address];
            return [NSString stringWithFormat:self.format, value];
        };
    }
    return self;
}

// XXX: yes, i am using bit field, if it does not work.
//      will use
// processor status (the P register)
union ProcessorStatus {
    struct Flags {
        bool carry     : 1;
        bool zero      : 1;
        bool interrupt : 1; // interrupt enabled/disable
        bool decimal   : 1; // not supported on N2A03
        bool breaks    : 1; // software interrupt,
        bool reserved  : 1; // unused, always set
        bool overflow  : 1;
        bool negative  : 1;
    } flags;
    uint8_t bits;
};

- (id)initAsReg:(NSString *)name {
    if (self = [super init]) {
        self.name = name;
        Reg reg = [Watched nameToReg:name];
        if (reg == SP || reg == PC) {
            self.format = @"%04x";
           _getValue = ^ NSString *(DebuggerBridge *debugger) {
               uint16_t value = [debugger peekReg:reg];
               return [NSString stringWithFormat:self.format, value];
           };
        } else if (reg == P) {
           // the Processor status register is kinda different
           self.format = @"N%d V%d -1 B%d D%d I%d Z%d C%d";
           _getValue = ^ NSString *(DebuggerBridge *debugger) {
                union ProcessorStatus p;
                p.bits = [debugger peekReg:reg];
                return [NSString stringWithFormat:self.format,
                        p.flags.negative,
                        p.flags.overflow,
                        p.flags.reserved,
                        p.flags.breaks,
                        p.flags.decimal,
                        p.flags.interrupt,
                        p.flags.zero,
                        p.flags.carry];
            };
        } else if (reg == UNKNOWN) {
            self.format = @"-";
            _getValue = ^ NSString *(DebuggerBridge *debugger) {
                return @"unknown register";
            };
        } else {
            self.format = @"%d";
            _getValue = ^ NSString *(DebuggerBridge *debugger) {
                uint8_t value = [debugger peekReg:reg];
                return [NSString stringWithFormat:self.format, value];
            };
        }
    }
    return self;
}

@end
