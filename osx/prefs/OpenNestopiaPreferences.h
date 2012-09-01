/* OpenNestopiaPreferences */

#import <Cocoa/Cocoa.h>
#import "DBPrefsWindowController.h"

@interface OpenNestopiaPreferences : DBPrefsWindowController
{
    IBOutlet NSView *audioPrefsView;
    IBOutlet NSView *controlsPrefsView;
    IBOutlet NSView *generalPrefsView;
    IBOutlet NSView *videoPrefsView;
}
@end
