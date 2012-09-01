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
#import "ButtonState.h"

@interface OpenNestopiaInputController (Private)

- (void) addEvent: (NSString *) event usageId: (unsigned) usageId;

@end


@implementation OpenNestopiaInputController

- (id) init;
{
    self = [super init];
    if (self == nil)
        return nil;
    
    mEvents = [[NSMutableArray alloc] init];
    
	NSString* test;
	test = [NSString stringWithString:@"Hello?"];
	  mPad1Controls = [[NSMutableArray alloc] init];
	//[mPad1Controls addObject:test];
    return self;
}

- (void) awakeFromNib;
{
    NSArray * keyboards = [DDHidKeyboard allKeyboards];
    [keyboards makeObjectsPerformSelector: @selector(setDelegate:)
                               withObject: nestopiaView];
    [self setKeyboards: keyboards];
    
    if ([keyboards count] > 0)
        [self setKeyboardIndex: 0];
    else
        [self setKeyboardIndex: NSNotFound];
	
	
	NSArray * joysticks = [DDHidJoystick allJoysticks];
	
    mJoystickButtons = [[NSMutableArray alloc] init];
    [joysticks makeObjectsPerformSelector: @selector(setDelegate:)
                               withObject: self];
    [self setJoysticks: joysticks];
    if ([mJoysticks count] > 0)
        [self setJoystickIndex: 0];
    else
        [self setJoystickIndex: NSNotFound];
}

//=========================================================== 
// dealloc
//=========================================================== 
- (void) dealloc
{
    [mKeyboards release];
    [mEvents release];
    
    mKeyboards = nil;
    mEvents = nil;
    [super dealloc];
}

//=========================================================== 
//  keyboards 
//=========================================================== 
- (NSArray *) keyboards
{
    return mKeyboards; 
}

- (void) setKeyboards: (NSArray *) theKeyboards
{
    if (mKeyboards != theKeyboards)
    {
        [mKeyboards release];
        mKeyboards = [theKeyboards retain];
    }
}
//=========================================================== 
//  keyboardIndex 
//=========================================================== 
- (unsigned) keyboardIndex
{
    return mKeyboardIndex;
}

- (void) setKeyboardIndex: (unsigned) theKeyboardIndex
{
    if (mCurrentKeyboard != nil)
    {
        [mCurrentKeyboard stopListening];
        mCurrentKeyboard = nil;
    }
    mKeyboardIndex = theKeyboardIndex;
    [mKeyboardsController setSelectionIndex: mKeyboardIndex];
    [self willChangeValueForKey: @"events"];
    [mEvents removeAllObjects];
    [self didChangeValueForKey: @"events"];
    if (mKeyboardIndex != NSNotFound)
    {
        mCurrentKeyboard = [mKeyboards objectAtIndex: mKeyboardIndex];
        [mCurrentKeyboard startListening];
    }
}

//=========================================================== 
//  joysticks 
//=========================================================== 
- (NSArray *) joysticks
{
    return mJoysticks; 
}

- (NSArray *) joystickButtons;
{
    return mJoystickButtons;
}


//=========================================================== 
//  joystickIndex 
//=========================================================== 
- (unsigned) joystickIndex
{
    return mJoystickIndex;
}

- (void) setJoystickIndex: (unsigned) theJoystickIndex
{
    if (mCurrentJoystick != nil)
    {
        [mCurrentJoystick stopListening];
        mCurrentJoystick = nil;
    }
    mJoystickIndex = theJoystickIndex;
    [mJoysticksController setSelectionIndex: mJoystickIndex];
    if (mJoystickIndex != NSNotFound)
    {
        mCurrentJoystick = [mJoysticks objectAtIndex: mJoystickIndex];
        [mCurrentJoystick startListening];
        
        [self willChangeValueForKey: @"joystickButtons"];
        [mJoystickButtons removeAllObjects];
        NSArray * buttons = [mCurrentJoystick buttonElements];
        NSEnumerator * e = [buttons objectEnumerator];
        DDHidElement * element;
        while (element = [e nextObject])
        {
            ButtonState * state = [[ButtonState alloc] initWithName: [[element usage] usageName]];
            [state autorelease];
            [mJoystickButtons addObject: state];
        }
        [self didChangeValueForKey: @"joystickButtons"];
    }
}


//=========================================================== 
//  events 
//=========================================================== 
- (NSMutableArray *) events
{
    return mEvents; 
}

- (void) setEvents: (NSMutableArray *) theEvents
{
    if (mEvents != theEvents)
    {
        [mEvents release];
        mEvents = [theEvents retain];
    }
}
- (void) addEvent: (id)theEvent
{
    [[self events] addObject: theEvent];
}
- (void) removeEvent: (id)theEvent
{
    [[self events] removeObject: theEvent];
}
- (void) setJoysticks: (NSArray *) theJoysticks
{
    if (mJoysticks != theJoysticks)
    {
        [mJoysticks release];
        mJoysticks = [theJoysticks retain];
    }
}
@end

@implementation OpenNestopiaInputController (DDHidJoystickDelegate)

- (void) ddhidJoystick: (DDHidJoystick *)  joystick
                 stick: (unsigned) stick
              xChanged: (int) value;
{
	NSLog(@"Stick: %d, XValue: %d",stick,value);
    [self willChangeValueForKey: @"xAxis"];
    mXAxis = value;
    [self didChangeValueForKey: @"xAxis"];
}

- (void) ddhidJoystick: (DDHidJoystick *)  joystick
                 stick: (unsigned) stick
              yChanged: (int) value;
{
	NSLog(@"Stick: %d, XValue: %d",stick,value);
    [self willChangeValueForKey: @"yAxis"];
    mYAxis = value;
    [self didChangeValueForKey: @"yAxis"];
}

- (void) ddhidJoystick: (DDHidJoystick *) joystick
                 stick: (unsigned) stick
             otherAxis: (unsigned) otherAxis
          valueChanged: (int) value;
{
    // Somehow display values here
    NSLog(@"Stick: %d, other axis: %d, changed: %d", stick, otherAxis, value);
}

- (void) ddhidJoystick: (DDHidJoystick *) joystick
                 stick: (unsigned) stick
             povNumber: (unsigned) povNumber
          valueChanged: (int) value;
{
    // Somehow display values here
    NSLog(@"Stick: %d, POV number: %d, changed: %d", stick, povNumber, value);
}

- (void) ddhidJoystick: (DDHidJoystick *) joystick
            buttonDown: (unsigned) buttonNumber;
{
	NSLog(@"Button :%d pressed.",buttonNumber);
    ButtonState * state = [mJoystickButtons objectAtIndex: buttonNumber];
    [state setPressed: YES];
}

- (void) ddhidJoystick: (DDHidJoystick *) joystick
              buttonUp: (unsigned) buttonNumber;
{
	NSLog(@"Button :%d pressed.",buttonNumber);
    ButtonState * state = [mJoystickButtons objectAtIndex: buttonNumber];
    [state setPressed: NO];
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





- (void) addEvent: (NSString *) event usageId: (unsigned) usageId;
{
	
    DDHidUsageTables * usageTables = [DDHidUsageTables standardUsageTables];
    NSString * description = [NSString stringWithFormat: @"%@ (0x%dne)",
        [usageTables descriptionForUsagePage: kHIDPage_KeyboardOrKeypad
                                       usage: usageId],
        usageId];
	//printf("%d\n",usageId);
	//NSLog(event);
    NSLog(description);
    NSMutableDictionary * row = [mKeyboardEventsController newObject];
    [row setObject: event forKey: @"event"];
    [row setObject: description forKey: @"description"];
    [mKeyboardEventsController addObject: row];
}

@end
