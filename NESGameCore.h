/*
 Copyright (c) 2009, OpenEmu Team
 
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the OpenEmu Team nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY OpenEmu Team ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL OpenEmu Team BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <Foundation/Foundation.h>

#include <OpenGL/gl.h>

#import "RenderDelegate.h"
#import "osx/ResponderClient.h"
#import "util.h"

extern NSString *const NESNTSC;
extern NSString *const NESBrightness;
extern NSString *const NESSaturation;
extern NSString *const NESContrast;
extern NSString *const NESSharpness;
extern NSString *const NESColorRes;
extern NSString *const NESColorBleed;
extern NSString *const NESColorArtifacts;
extern NSString *const NESColorFringing ;
extern NSString *const NESHue;
extern NSString *const NESUnlimitedSprites;

extern NSString *const NESEmulatorDidPauseNotification;
extern NSString *const NESEmulatorDidResumeNotification;

#pragma mark -

@class OERingBuffer;

typedef BOOL (^ExecCondition)(void);

@interface NESGameCore : NSObject <NESSystemResponderClient>
{
    NSUInteger             frameSkip;
    NSUInteger             frameCounter;

    BOOL                   frameFinished;
    BOOL                   willSkipFrame;
    
    BOOL                   isRunningChanged;
    BOOL                   isRunning;
    BOOL                   shouldStop;

    NSLock                *soundLock;
    __strong OERingBuffer *_ringBuffer;
    UInt16                *soundBuffer;

    NSString              *romPath;

    NSLock               *videoLock;
    int                   bufFrameSize;
    NSUInteger            width;
    NSUInteger            height;
    const unsigned char  *videoBuffer;
    
    void                 *_emu;

    void                 *_nesSound;
    void                 *_nesVideo;
    void                 *_controls;
}

@property(copy) NSString *romPath;
@property(readonly) NSString *batterySavesDirectoryPath;
@property(readonly) OERingBuffer *ringBuffer;
@property(readonly) void *nesEmu;
@property(readonly) NSUInteger pc;
@property(readonly) NSUInteger audioBufferSize;
@property(readonly) double audioSampleRate;
@property(readonly) NSUInteger channelCount;
@property(readonly) const unsigned char *videoBuffer;

@property(readonly) GLenum pixelFormat;
@property(readonly) GLenum pixelType;
@property(readonly) GLenum internalPixelFormat;
@property(readonly) IntSize bufferSize;

@property(weak)     id<RenderDelegate>  renderDelegate;
@property(copy)     ExecCondition execCondition;

- (void)executeFrame;

#pragma mark -
#pragma mark Execution
@property(getter=isEmulationPaused) BOOL pauseEmulation;
- (void)setupEmulation;
- (void)resetEmulation;
- (void)stopEmulation;
- (void)startEmulation;

- (BOOL)loadFileAtPath:(NSString*) path;

- (BOOL)lockVideo:(void *)video;
- (void)unlockVideo:(void *)video;

- (BOOL)lockSound;
- (void)unlockSound;

- (void)setupVideo:(void *)emulator withFilter:(int)filter;

- (BOOL)saveStateToFileAtPath:(NSString *)fileName;
- (BOOL)loadStateFromFileAtPath:(NSString *)fileName;

@end

#pragma mark NES-specific features
@interface NESGameCore (NesAdditions)

- (BOOL)isUnlimitedSpritesEnabled;
- (int)brightness;
- (int)saturation;
- (int)contrast;
- (int)sharpness;
- (int)colorRes;
- (int)colorBleed;
- (int)colorArtifacts;
- (int)colorFringing;
- (int)hue;

- (void)applyNTSC:(id)sender;
- (BOOL)isNTSCEnabled;

- (void)toggleUnlimitedSprites:(id)sender;
- (void)enableUnlimitedSprites:(BOOL)enable;
- (void)setCode:(NSString *)code;
- (void)enableRewinder:(BOOL)rewind;
- (BOOL)isRewinderEnabled;
- (void)rewinderDirection:(NSUInteger)rewinderDirection;
- (void)enableRewinderBackwardsSound:(BOOL)rewindSound;
- (BOOL)isRewinderBackwardsSoundEnabled;

- (int)cartVRamSize;
- (int)chrRomSize;

- (void) setNmtRamBytes:(double)off value:(double)val;
- (void) setChrRamBytes:(double)off value:(double)val;


@end
