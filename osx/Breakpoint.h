//
//  Breakpoint.h
//  OpenNestopia
//
//  Created by Kefu Chai on 16/09/12.
//
//

#import <Foundation/Foundation.h>

typedef enum : NSUInteger {
    AccessNone  = 0,
    AccessRead  = 1 << 0,
    AccessWrite = 1 << 1,
    AccessExec = 1 << 2,
    AccessRW = AccessRead | AccessWrite,
} AccessMode;
    
@interface Breakpoint : NSObject

- (id)initWithAddress:(NSUInteger)address
               access:(AccessMode)access
              enabled:(BOOL)enabled;
- (NSString *)triggerDescAt:(NSUInteger)pc;
+ (NSString *)descriptionHeader;

@property (nonatomic, assign) NSUInteger index;
@property (nonatomic, assign) BOOL enabled;
@property (nonatomic, assign) NSUInteger address;
@property (nonatomic, assign) AccessMode access;

@end
