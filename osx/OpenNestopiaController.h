/* OpenNestopiaController */

#import <Cocoa/Cocoa.h>
#import "NestopiaView.h"

@interface OpenNestopiaController : NSObject
{
	
	
    NSToolbar *toolbar;
    
    NSMutableDictionary *items; // all items that are allowed to be in the toolbar
	
	IBOutlet NSMatrix *gameSharkCode;
	
	IBOutlet NSMatrix *dipSwitchLabels;
	IBOutlet NSPanel *dipSwitchPanel;
	
	IBOutlet NSWindow *mainWindow;
	IBOutlet NestopiaView *nestopiaView;
}


- (IBAction)applyCheats:(id)sender;
- (IBAction)openPreferencesWindow:(id)sender;
- (IBAction)testDips:(id)sender;
- (IBAction)buttonChanged:(id)sender;

void TestDefaultAU();
void CreateDefaultAU();

@end
