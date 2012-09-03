#import "NestopiaView.h"

#import <DDHidLib/lib/DDHidLib.h>
#import "core/api/NstApiInput.hpp"

#include <iostream>
#include <fstream>
#include <math.h>
#include <sys/time.h>

#import "util.h"
#import "OEGameAudio.h"
#import "NESGameCore.h"


@implementation NestopiaView

NSString* saveName;

bool fullscreen = false;
bool playing = false;
bool fGLSetup = false;

NSString* fName;

//NSConditionLock cLock;


-(void)resetVideo
{
    if (fGLSetup) {
        NSRect f;
        if(!fullscreen) {
            f = [self frame];
            glViewport(0.0, 0.0, f.size.width,f.size.height);
        } else
            f = [[NSScreen mainScreen] frame];
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0.0,f.size.width,f.size.height, 0.0, 0.0, -1.0);
    }
}

- (BOOL)isFlipped 
{ 
    return YES; 
}

- (void)copyGLToBackingStore
{
    //NSLog(@"Copy backing");
    [[self openGLContext] makeCurrentContext]; 
    
    NSSize size = [self bounds].size; 
    
    void *buffer = malloc(size.width * size.height * 4); 
    
    glReadPixels(0, 
                 0, 
                 size.width, 
                 size.height, 
                 GL_RGBA, 
                 GL_UNSIGNED_BYTE, 
                 buffer); 
    
    [self lockFocus]; 
    
    NSDrawBitmap([self bounds], 
                 size.width, 
                 size.height, 
                 8, 
                 4, 
                 32, 
                 size.width * 4, 
                 NO, 
                 NO, 
                 NSDeviceRGBColorSpace, 
                 (unsigned char const **)&buffer); 
    
    [self unlockFocus]; 
    
    free(buffer);    
}

-(void)powerOff
{
    [gameCore stopEmulation];
}

-(void)resetGame
{
    [gameCore resetEmulation];
}

#pragma mark --Cocoa overloads--

- (id)initWithFrame:(NSRect)frameRect
{        
    
    
    NSOpenGLPixelFormatAttribute MyAttributes[] =
    {
        NSOpenGLPFAWindow,
        NSOpenGLPFASingleRenderer,
        NSOpenGLPFANoRecovery,
        NSOpenGLPFAScreenMask,
        (NSOpenGLPixelFormatAttribute)CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
        NSOpenGLPFADoubleBuffer,
        (NSOpenGLPixelFormatAttribute)0
    };
    
    NSOpenGLPixelFormat* pixelFormat =
        [[NSOpenGLPixelFormat alloc] initWithAttributes:MyAttributes];
    if (pixelFormat == nil) {
        return nil;
    }
    
    MyAttributes[0] = NSOpenGLPFAFullScreen;
    NSOpenGLPixelFormat* fullScreenPixelFormat =
        [[NSOpenGLPixelFormat alloc] initWithAttributes:MyAttributes];
    if (fullScreenPixelFormat == nil) {
        return nil;
    }
    
    self = [super initWithFrame:frameRect pixelFormat:pixelFormat];
    if (self == nil) {
        return nil;
    }
    
    fullScreenGLContext =
        [[NSOpenGLContext alloc] initWithFormat:fullScreenPixelFormat
                                   shareContext:[self openGLContext]];
    if (fullScreenGLContext == nil) {
        return nil;
    }
    
    fullscreen = NO;
    //NSLog(@"init");
    return self;
}

-(IBAction)startLoad:(id)sender
{
    NSOpenPanel* oPanel = [NSOpenPanel openPanel];
    oPanel.canChooseDirectories = NO;
    oPanel.canChooseFiles = YES;
    oPanel.canCreateDirectories = NO;
    oPanel.allowsMultipleSelection = NO;
    oPanel.alphaValue = 0.95;
    oPanel.title = @"Nestopia: Load";
    oPanel.allowedFileTypes = @[@"nes"];
    
    if ([oPanel runModal] == NSOKButton) {
        if (self.loadedRom) {
            [self stopEmulation];
            self.loadedRom = NO;
        }
        gameCore = [[NESGameCore alloc] init];
        gameCore.renderDelegate = self;
        NSString* fileName = [[oPanel URL] path];
        saveName = [[[fileName lastPathComponent] stringByDeletingPathExtension] stringByAppendingPathExtension:@"sav"];
        NSLog(@"load saved state: %@", fileName);
        self.loadedRom = [gameCore loadFileAtPath:fileName];
    }
    
    if(sender!=nil) {
        [mainWindow makeKeyAndOrderFront:nil];
        [self setupEmulation];
    }
}

- (void)setupEmulation {
    NSLog(@"Setting up emulation");
    
    [gameCore setupEmulation];
    
    // audio!
    gameAudio = [[OEGameAudio alloc] initWithCore:gameCore];
    
    [self setupGameCore];
    
    gameCoreThread = [[NSThread alloc] initWithTarget:self selector:@selector(OE_gameCoreThread:) object:nil];
//    [gameCoreProxy setGameThread:gameCoreThread];
    [gameCoreThread start];
    
    DLog(@"finished starting rom");
}

- (void)stopEmulation
{
    [gameCore stopEmulation];
    [gameAudio stopAudio];
    [gameCore setRenderDelegate:nil];
    gameCore      = nil;
    gameAudio     = nil;
    
    if (gameCoreThread != nil) {
        [self performSelector:@selector(OE_stopGameCoreThreadRunLoop:) onThread:gameCoreThread withObject:nil waitUntilDone:YES];
        gameCoreThread = nil;
    }
}

- (void)OE_gameCoreThread:(id)anObject;
{
    NSLog(@"Begin separate thread");
    
    // starts the threaded emulator timer
    [gameCore startEmulation];
    
    CFRunLoopRun();
    
    NSLog(@"Did finish separate thread");
}

- (void)OE_stopGameCoreThreadRunLoop:(id)anObject
{
    CFRunLoopStop(CFRunLoopGetCurrent());
    
    NSLog(@"Finishing separate thread");
}


- (void)setupGameCore {
    [gameAudio setVolume:1.0];
    [self setupGameTexture];
}

-(void)awakeFromNib
{
    [self startLoad:nil];
    
    NSRect oldFrame = [mainWindow frame];
    NSRect newFrame = [self frame];
    newFrame.origin = oldFrame.origin;
    IntSize bufferSize = gameCore.bufferSize;
    NSSize aspect = NSMakeSize(bufferSize.width, bufferSize.height);
    newFrame.size = NSMakeSize(bufferSize.width, bufferSize.height);
    
    [mainWindow setContentAspectRatio:aspect];
    
    [mainWindow setContentSize:newFrame.size];    
    [mainWindow makeKeyAndOrderFront:nil];
    [self setupEmulation];
    [self resetVideo];
}

-(void)setFrame:(NSRect)frame
{
    [super setFrame:frame];
    [self resetVideo];
}

-(void)setupGameTexture
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glHint(GL_TRANSFORM_HINT_APPLE, GL_FASTEST);
    glEnable(GL_TEXTURE_RECTANGLE_EXT);
    glGenTextures(1, &gameTexture);
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, gameTexture);
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);

    // proper tex params.
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    DLog(@"set params - uploading texture");
    IntSize bufferSize = gameCore.bufferSize;
    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0,
                 gameCore.internalPixelFormat,
                 bufferSize.width, bufferSize.height, 0,
                 gameCore.pixelFormat,
                 gameCore.pixelType,
                 gameCore.videoBuffer);
    glClearColor (0.0, 0.0, 1.0, 1.0);
    glClearDepth(1.0);
    
    fGLSetup = true;
}

- (void) ddhidKeyboard: (DDHidKeyboard *) keyboard
                 keyUp: (unsigned) usageId;
{
    if ([mainWindow isKeyWindow]) {
        switch (usageId) {
            case 82:
                [gameCore didReleaseNESButton:NESButtonUp forPlayer:0];
                break;
            case 79:
                [gameCore didReleaseNESButton:NESButtonRight forPlayer:0];
                break;
            case 81:
                [gameCore didReleaseNESButton:NESButtonDown forPlayer:0];
                break;
            case 80:
                [gameCore didReleaseNESButton:NESButtonLeft forPlayer:0];
                break;
            case 4:
                [gameCore didReleaseNESButton:NESButtonA forPlayer:0];
                break;
            case 22:
                [gameCore didReleaseNESButton:NESButtonB forPlayer:0];
                break;
            case 40:
                [gameCore didReleaseNESButton:NESButtonStart forPlayer:0];
                break;    
            case 49:
                [gameCore didReleaseNESButton:NESButtonSelect forPlayer:0];
                break;
        }
    }
     NSLog(@"Keyboard log: %d",usageId);
    //   [self addEvent: @"Key Down" usageId: usageId];
}

- (void) ddhidKeyboard: (DDHidKeyboard *) keyboard
               keyDown: (unsigned) usageId;
{
    if([mainWindow isKeyWindow])
    {
        
        switch(usageId)
        {
            case 82:
                [gameCore didPushNESButton:NESButtonUp forPlayer:0];
                break;
            case 79:
                [gameCore didPushNESButton:NESButtonRight forPlayer:0];
                break;
            case 81:
                [gameCore didPushNESButton:NESButtonDown forPlayer:0];
                break;
            case 80:
                [gameCore didPushNESButton:NESButtonLeft forPlayer:0];
                break;
            case 4:
                [gameCore didPushNESButton:NESButtonA forPlayer:0];
                break;
            case 22:
                [gameCore didPushNESButton:NESButtonB forPlayer:0];
                break;
            case 40:
                [gameCore didPushNESButton:NESButtonStart forPlayer:0];
                break;
            case 49:
                [gameCore didPushNESButton:NESButtonSelect forPlayer:0];
                break;
        }
    }
}


- (IBAction)fullscreenToggle:(id)sender
{
    if (0) {
        [NSCursor unhide];
        [NSMenu setMenuBarVisible:YES];
        [fullScreenGLContext clearDrawable];
        [[self openGLContext] makeCurrentContext];
        CGReleaseAllDisplays();
        [self resetVideo];
    } else {
        CGCaptureAllDisplays();
        [fullScreenGLContext setFullScreen];
        [fullScreenGLContext makeCurrentContext];
        fullscreen = YES;
        [self resetVideo];        
        [NSMenu setMenuBarVisible:NO];
        [NSCursor hide];
    }
}


- (void)mouseMoved:(NSEvent *)theEvent {
}

- (void)mouseDown:(NSEvent *)theEvent  {
}

-(void)mouseUp:(NSEvent *)theEvent {
}

// this is called whenever the view changes (is unhidden or resized)
- (void)drawRect:(NSRect)frameRect 
{
    [self render];
}


- (BOOL)acceptsFirstResponder
{
    return YES;
}

-(void) loadState
{
    NSOpenPanel* oPanel = [NSOpenPanel openPanel];
    oPanel.canChooseDirectories = NO;
    oPanel.canChooseFiles = YES;
    oPanel.canCreateDirectories = NO;
    oPanel.allowsMultipleSelection = NO;
    oPanel.alphaValue = 0.95;
    oPanel.allowedFileTypes = @[@"save"];
    oPanel.title = @"Load State";

    if ([oPanel runModal] == NSOKButton) {
        NSLog(@"load state from: %@", oPanel.URL.path);
        [gameCore loadStateFromFileAtPath:oPanel.URL.path];
    }
}

- (void)saveState
{
    NSSavePanel* sPanel = [NSSavePanel savePanel];
    sPanel.canCreateDirectories = YES;
    sPanel.alphaValue = 0.95;
    sPanel.title = @"Save State";
    sPanel.allowedFileTypes = @[@"sav"];

    if( [sPanel runModal] == NSOKButton) {
        [gameCore saveStateToFileAtPath:sPanel.URL.path];
    }
}

- (void)keyDown:(NSEvent *)theEvent 
{
    
}

#pragma mark -
#pragma mark RenderDelegate protocol methods

- (void)render {
    [[self openGLContext] makeCurrentContext];
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_TEXTURE_RECTANGLE_ARB);

    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, gameTexture);
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    IntSize bufferSize = gameCore.bufferSize;
    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0,
                 gameCore.internalPixelFormat,
                 bufferSize.width, bufferSize.height, 0,
                 gameCore.pixelFormat,
                 gameCore.pixelType,
                 gameCore.videoBuffer);

    GLenum status = glGetError();
    if(status)
    {
        NSLog(@"updateGameTexture, after updating tex: OpenGL error %04X", status);
        glDeleteTextures(1, &gameTexture);
        gameTexture = 0;
    }
    NSRect f = [self bounds];
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 0);
        glVertex2f(0, 0);

        glTexCoord2f(0, bufferSize.height);
        glVertex2f(0, NSHeight(f));

        glTexCoord2f(bufferSize.width, bufferSize.height);
        glVertex2f(NSWidth(f), NSHeight(f));

        glTexCoord2f(bufferSize.width, 0);
        glVertex2f(NSWidth(f), 0);
    }
    glEnd();
    [self.openGLContext flushBuffer];
    //glFlush();
    //[[NSOpenGLContext currentContext] flushBuffer];
}

- (void)endDraw {
    // flush to make sure IOSurface updates are seen in parent app.
    glFlushRenderAPPLE();
}

- (void)willExecute
{
    [self render];
}

- (void)didExecute
{
    [self endDraw];
    
    if (!hasStartedAudio)
    {
        [gameAudio startAudio];
        hasStartedAudio = YES;
    }
}


@end
