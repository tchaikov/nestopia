/*
 * Copyright (c) 2007 Dave Dribin
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#import "OpenNestopiaInputController.h"

@interface OpenNestopiaInputController (Private)

- (void) addEvent: (NSString *) event usageId: (unsigned) usageId;

@end


@implementation OpenNestopiaInputController


- (id) init;
{
    self = [super init];
    if (self == nil)
        return nil;
    self.events = [[NSMutableArray alloc] init];
    return self;
}

- (void) awakeFromNib;
{
    NSArray * keyboards = [DDHidKeyboard allKeyboards];
    for (DDHidKeyboard *kb in keyboards) {
        kb.delegate = nestopiaView;
    }
    self.keyboards = keyboards.mutableCopy;
    if (keyboards.count > 0)
        self.keyboardIndex = 0;
    else
        self.keyboardIndex = NSNotFound;
}

//=========================================================== 
// dealloc
//=========================================================== 
- (void) dealloc {
}

- (void) setKeyboardIndex: (NSInteger) theKeyboardIndex
{
    if (mCurrentKeyboard != nil) {
        [mCurrentKeyboard stopListening];
        mCurrentKeyboard = nil;
    }
    _keyboardIndex = theKeyboardIndex;
    [mKeyboardsController setSelectionIndex: self.keyboardIndex];
    [self willChangeValueForKey: @"events"];
    [self.events removeAllObjects];
    [self didChangeValueForKey: @"events"];
    if (self.keyboardIndex != NSNotFound) {
        mCurrentKeyboard = self.keyboards[self.keyboardIndex];
        [mCurrentKeyboard startListening];
    }
}

//=========================================================== 
//  events 
//=========================================================== 
- (void) addEvent: (id)theEvent
{
    [self.events addObject: theEvent];
}

- (void) removeEvent: (id)theEvent
{
    [self.events removeObject: theEvent];
}

@end


@implementation OpenNestopiaInputController (DDHidKeyboardDelegate)

- (void) ddhidKeyboard: (DDHidKeyboard *) keyboard
               keyDown: (unsigned) usageId;
{
    [self addEvent: @"Key Down" usageId: usageId];
}

- (void) ddhidKeyboard: (DDHidKeyboard *) keyboard
                 keyUp: (unsigned) usageId;
{
    [self addEvent: @"Key Up" usageId: usageId];
}

@end


@implementation OpenNestopiaInputController (Private)

- (void)addEvent:(NSString *)event usageId:(unsigned) usageId;
{
    DDHidUsageTables * usageTables = [DDHidUsageTables standardUsageTables];
    NSString * description = [NSString stringWithFormat: @"%@ (0x%dne)",
        [usageTables descriptionForUsagePage: kHIDPage_KeyboardOrKeypad
                                       usage: usageId],
        usageId];
    NSLog(@"addEvent: %@", description);
    NSMutableDictionary * row = [mKeyboardEventsController newObject];
    row[@"event"] = event;
    row[@"description"] = description;
    [mKeyboardEventsController addObject: row];
}

@end
