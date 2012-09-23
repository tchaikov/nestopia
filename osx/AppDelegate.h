// AppDelegate.h
// OpenNestopia
//
//  Created by Kefu Chai on 23/09/12.
//  Copyright (c) 2012 Kefu Chai. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NestopiaView.h"

@class DebuggerWindowController;

@interface AppDelegate : NSObject<NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *mainWindow;
@property (assign) IBOutlet NestopiaView *nesView;
@property (assign) IBOutlet DebuggerWindowController *debugerWindowController;

- (IBAction)openPreferencesWindow:(id)sender;
- (IBAction)openDebugConsole:(id)sender;

@end
