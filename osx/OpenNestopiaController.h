/* OpenNestopiaController */

#import <Cocoa/Cocoa.h>
#import "NestopiaView.h"

@class DebuggerWindowController;

@interface OpenNestopiaController : NSObject
{
    NSToolbar *toolbar;
    
    NSMutableDictionary *items; // all items that are allowed to be in the toolbar
	
	IBOutlet NSMatrix *gameSharkCode;
	
	IBOutlet NSMatrix *dipSwitchLabels;
	IBOutlet NSPanel *dipSwitchPanel;
	
	IBOutlet NSWindow *mainWindow;
	IBOutlet NestopiaView *nesView;
    IBOutlet DebuggerWindowController *debugerWindowController;
}

- (IBAction)openPreferencesWindow:(id)sender;
- (IBAction)openDebugConsole:(id)sender;

@end
