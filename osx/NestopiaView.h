/* NestopiaView */
#import <AppKit/AppKit.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <Cocoa/Cocoa.h>

#import "RenderDelegate.h"

NSLock *soundLock;

@class NESGameCore;
@class OEGameAudio;

@interface NestopiaView : NSOpenGLView<RenderDelegate>
{
	
	IBOutlet NSWindow *mainWindow;
    IBOutlet NSToolbarItem *toolbarItem;

	NSOpenGLContext *fullScreenGLContext;
    GLuint           gameTexture;      // this is the texture that is defined by the gameCores pixelFormat and type

    NSThread    *gameCoreThread;
    NESGameCore *_gameCore;
    OEGameAudio *gameAudio;
    BOOL         hasStartedAudio;
}

@property (nonatomic, readonly) NESGameCore *gameCore;
@property (assign) BOOL loadedRom;
@property (nonatomic) id pausedObserver;
@property (nonatomic) id resumedObserver;

- (void)drawRect:(NSRect)frameRect;
- (void)setFrame:(NSRect)frame;
- (void)copyGLToBackingStore;
- (id)initWithFrame:(NSRect)frame;
- (void)resetGame;
- (void)powerOff;

- (IBAction)togglePlayPause:(id)sender;
- (IBAction)reset:(id)sender;
- (IBAction)stop:(id)sender;
- (IBAction)loadRom:(id)sender;
- (IBAction)loadState:(id)sender;
- (IBAction)saveState:(id)sender;
- (IBAction)fullscreenToggle:(id)sender;

- (void)resetVideo;

@end
