/* NestopiaView */
#import <AppKit/AppKit.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <Cocoa/Cocoa.h>
	
NSLock *soundLock;

@interface NestopiaView : NSOpenGLView
{
	
	IBOutlet NSWindow *mainWindow;
	IBOutlet NSPanel *dipSwitchPanel;
	
	NSOpenGLContext *fullScreenGLContext;
	
	NSTimer *gameTimer;

}

- (int)getSelectedValue:(int)num;
- (void)setDipValuesForDip:(int)dip value:(int)value;
- (NSArray*)getDipValues:(int)num;
- (NSString*)getDipName:(int)num;
- (int)numOfDips;
- (void) saveState;
- (void) dealloc;
- (void)drawRect:(NSRect)frameRect;
- (void)setFrame:(NSRect)frame;
- (void)copyGLToBackingStore;
- (void)setupGL;
- (void)setCode:(NSString*)code;
- (void)awakeFromNib;
- (id)initWithFrame:(NSRect)frame;
- (void)resetGame;
- (void)powerOff;
- (void)loadDatabase;
- (void)startTimer:(float)fps;
- (void)testBarcode;
- (void)executeFrame;
- (IBAction)startLoad:(id)sender;
- (IBAction)fullscreenToggle:(id)sender;

- (void)resetVideo;
- (int)getHeight;
- (int)getWidth;

- (void)setupSound;
- (void)setupVideo;
- (void)setupInput;


@end
