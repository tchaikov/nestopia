#import "NestopiaView.h"

#import <AudioToolbox/AUGraph.h>
#import "DDCoreAudio/DDCoreAudio.h"
#include <AudioUnit/AudioUnit.h>
#import <DDHidLib/DDHidLib.h>

#include <iostream>
#include <fstream>
#include <math.h>
#include <sys/time.h>

#include "../core/api/NstApiEmulator.hpp"
#include "../core/api/NstApiVideo.hpp"
#include "../core/api/NstApiSound.hpp"
#include "../core/api/NstApiInput.hpp"
#include "../core/api/NstApiMachine.hpp"
#include "../core/api/NstApiCartridge.hpp"
#include "../core/api/NstApiUser.hpp"
#include "../core/api/NstApiCheats.hpp"
#include "../core/api/NstApiDipSwitches.hpp"
#include "../core/api/NstApiBarcodeReader.hpp"
#include "../zlib/unzip.h"

static bool NST_CALLBACK SoundLock(void* userData,Nes::Api::Sound::Output& sound);
static void NST_CALLBACK SoundUnlock(void* userData,Nes::Api::Sound::Output& sound);
static bool NST_CALLBACK VideoLock(void* userData,Nes::Api::Video::Output& video);
static void NST_CALLBACK VideoUnlock(void* userData,Nes::Api::Video::Output& video);
void	TestDefaultAU();
void	CreateDefaultAU();

@implementation NestopiaView

AudioUnit	gOutputUnit;

Nes::Api::Emulator emulator;


Nes::Api::Video::Output* nstVideo; 
Nes::Api::Sound::Output* nstSound;

NSString* saveName;

//AudioConverterRef converter;
//AudioStreamBasicDescription	destformat;
//bool drawable = true;
//bool screen1 = true;
bool soundReady = false;
#define UPDATE_INTERVAL 0.5
bool isPlaying = false;
static Nes::Api::Input::Controllers controls;
static Nes::Api::Cartridge::Database::Entry dbentry;

static void NST_CALLBACK DoFileIO(void* userData,Nes::Api::User::File operation,Nes::Api::User::FileData& data);

static unsigned char* videoScreen;

static unsigned short soundBuffer[0x8000];
static UInt32 bufInPos, bufOutPos, bufUsed;
static UInt32 sampleRate, bufSize, bufFrameSize;
char    fpsString[32];

bool fullscreen = false;
bool playing = false;
bool fGLSetup = false;
static int cur_width, cur_height, framerate;

NSString* fName;
char* cFileName;
//NSConditionLock cLock;


// the MTCoreAudioDevice IO target for playback.  We feed data from
// our buffer into the sound system

double CurrentTime(void)
{
    struct timeval time;
	
    gettimeofday(&time, NULL);
    return (double)time.tv_sec + (0.000001 * (double)time.tv_usec);
}

OSStatus	MyRenderer(void 				*inRefCon, 
					   AudioUnitRenderActionFlags 	*ioActionFlags, 
					   const AudioTimeStamp 		*inTimeStamp, 
					   UInt32 						inBusNumber, 
					   UInt32 						inNumberFrames, 
					   AudioBufferList 			*ioData)
{
	
//	if([soundLock tryLock])
//	{
	
		memset(ioData->mBuffers[0].mData,0,sizeof(ioData->mBuffers[0].mData));
		[soundLock lock];
		int i;
		
		if (bufUsed < inNumberFrames)
		{
			bufUsed = inNumberFrames;
			bufOutPos = (bufInPos + bufSize - inNumberFrames) % bufSize;
		}
		
		short *out = (short *)ioData->mBuffers[0].mData;
		
		for(i=0;i<inNumberFrames;++i)
		{
			*out++ = soundBuffer[bufOutPos];
			*out++ = soundBuffer[bufOutPos];
			bufOutPos = (bufOutPos + 1) % bufSize;
		}
			
		bufUsed -= inNumberFrames;
		[soundLock unlock];
		
	
 	return noErr;
}

// initialize input going into the game
-(void)setupInput
{
	Nes::Api::Cartridge::Database database( emulator );
	
	if(database.IsLoaded())
	{
		Nes::Api::Input(emulator).AutoSelectControllers(); // autoselect controllers for all five ports
														   //printf("\nloading controls from db?\n");
	}
	else
	{
		//printf("\nloadin from me\n");
		Nes::Api::Input(emulator).ConnectController( 0, Nes::Api::Input::PAD1 );
		Nes::Api::Input(emulator).ConnectController( 1, Nes::Api::Input::ZAPPER );
	}
}



-(void)setupVideo
{
	
	// renderstate structure
	Nes::Api::Video::RenderState renderState;
	nstVideo = new Nes::Api::Video::Output;
	int filter = [[NSUserDefaults standardUserDefaults] integerForKey:@"filter"]; 
	
	Nes::Api::Video::RenderState::Filter filters[7] = 
	{ 
		Nes::Api::Video::RenderState::FILTER_NONE, 
		Nes::Api::Video::RenderState::FILTER_NTSC, 
		Nes::Api::Video::RenderState::FILTER_SCALE2X, 
		Nes::Api::Video::RenderState::FILTER_SCALE3X, 
		Nes::Api::Video::RenderState::FILTER_HQ2X, 
		Nes::Api::Video::RenderState::FILTER_HQ3X, 
		Nes::Api::Video::RenderState::FILTER_HQ4X 
	};
	int Widths[7] =
	{
		Nes::Api::Video::Output::WIDTH,
		Nes::Api::Video::Output::NTSC_WIDTH,
		Nes::Api::Video::Output::WIDTH*2,
		Nes::Api::Video::Output::WIDTH*3,
		Nes::Api::Video::Output::WIDTH*2,
		Nes::Api::Video::Output::WIDTH*3,
		Nes::Api::Video::Output::WIDTH*4,
	};
	int Heights[7] =
	{
		Nes::Api::Video::Output::HEIGHT,
		Nes::Api::Video::Output::NTSC_HEIGHT,
		Nes::Api::Video::Output::HEIGHT*2,
		Nes::Api::Video::Output::HEIGHT*3,
		Nes::Api::Video::Output::HEIGHT*2,
		Nes::Api::Video::Output::HEIGHT*3,
		Nes::Api::Video::Output::HEIGHT*4,
	};
	
	Nes::Api::Machine machine( emulator );
	Nes::Api::Cartridge::Database database( emulator );
	
	// figure out the region
	framerate = 60;
	int vmode = 1;
	machine.SetMode(Nes::Api::Machine::NTSC);
	if (vmode == 2)		// force PAL
	{
		machine.SetMode(Nes::Api::Machine::PAL);
		framerate = 50;
	}
	else if (vmode == 1) 	// force NTSC
	{
		machine.SetMode(Nes::Api::Machine::NTSC);
	}
	else	// auto
	{
		if (database.IsLoaded())
		{
			//printf("Db loaded?");
			if (database.GetRegion(dbentry) == Nes::Api::Cartridge::REGION_PAL)
			{
				machine.SetMode(Nes::Api::Machine::PAL);
				framerate = 50;
			}
			else
			{
				machine.SetMode(Nes::Api::Machine::NTSC);
			}
		}
		else
		{
			
			machine.SetMode(machine.GetDesiredMode());
		}
	}
	
	cur_width =Widths[filter]; 
	cur_height = Heights[filter];
	
	// example configuration
	renderState.bits.count = 16;
	renderState.bits.mask.r = 0x7C00;
	renderState.bits.mask.g = 0x3E0;
	renderState.bits.mask.b = 0x1F;
	renderState.filter = filters[filter];
	renderState.width = Widths[filter];
	renderState.height = Heights[filter];
	//	renderState.scanlines = 25;
	
	videoScreen = new unsigned char[cur_width * cur_height * 3];
	//videoScreen2 = new unsigned char[cur_width * cur_height * 3];
	// acquire the video interface
	Nes::Api::Video video( emulator );
	
	// set the sprite limit
	video.EnableUnlimSprites(1 ? false : true);
	
	// set up the NTSC type
	switch (0)
	{
		case 0:	// composite
			video.SetSharpness(Nes::Api::Video::DEFAULT_SHARPNESS_COMP);
			video.SetColorResolution(Nes::Api::Video::DEFAULT_COLOR_RESOLUTION_COMP);
			video.SetColorBleed(Nes::Api::Video::DEFAULT_COLOR_BLEED_COMP);
			video.SetColorArtifacts(Nes::Api::Video::DEFAULT_COLOR_ARTIFACTS_COMP);
			video.SetColorFringing(Nes::Api::Video::DEFAULT_COLOR_FRINGING_COMP);
			break;
			
		case 1:	// S-Video
			video.SetSharpness(Nes::Api::Video::DEFAULT_SHARPNESS_SVIDEO);
			video.SetColorResolution(Nes::Api::Video::DEFAULT_COLOR_RESOLUTION_SVIDEO);
			video.SetColorBleed(Nes::Api::Video::DEFAULT_COLOR_BLEED_SVIDEO);
			video.SetColorArtifacts(Nes::Api::Video::DEFAULT_COLOR_ARTIFACTS_SVIDEO);
			video.SetColorFringing(Nes::Api::Video::DEFAULT_COLOR_FRINGING_SVIDEO);
			break;
			
		case 2:	// RGB
			video.SetSharpness(Nes::Api::Video::DEFAULT_SHARPNESS_RGB);
			video.SetColorResolution(Nes::Api::Video::DEFAULT_COLOR_RESOLUTION_RGB);
			video.SetColorBleed(Nes::Api::Video::DEFAULT_COLOR_BLEED_RGB);
			video.SetColorArtifacts(Nes::Api::Video::DEFAULT_COLOR_ARTIFACTS_RGB);
			video.SetColorFringing(Nes::Api::Video::DEFAULT_COLOR_FRINGING_RGB);
			break;
	}
	
	// set the render state, make use of the NES_FAILED macro, expands to: "if (function(...) < Nes::RESULT_OK)"	
	if (NES_FAILED(video.SetRenderState( renderState )))
	{
		printf("NEStopia core rejected render state\n");
		exit(0);
	}
}

-(void)resetVideo
{
	if(fGLSetup)
	{
		NSRect f;
		if(!fullscreen)
		{
			f = [self frame];
			glViewport(0.0, 0.0, f.size.width,f.size.height);
		}
		else
			f = [[NSScreen mainScreen] frame];
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glOrtho(0.0,f.size.width,f.size.height, 0.0, 0.0, -1.0);
	}
}

- (void)setupSound
{
	Nes::Api::Sound sound( emulator );
	sound.SetSampleBits( 16 );
	sound.SetSampleRate(sampleRate);
	sound.SetVolume(Nes::Api::Sound::ALL_CHANNELS, 50);
	sound.SetSpeaker( Nes::Api::Sound::SPEAKER_MONO );

	
    DDAudioUnitGraph * mGraph;
	
    DDAudioUnitNode * mConverterNode;
    DDAudioUnitNode * mEffectNode;
    DDAudioUnitNode * mOutputNode;
	
    DDAudioUnit * mConverterUnit;
    BOOL mEffectEnabled;
	
    unsigned mBytesPerFrame;
	
	mGraph = [[DDAudioUnitGraph alloc] init];
    mOutputNode = [mGraph addNodeWithType: kAudioUnitType_Output
                                  subType: kAudioUnitSubType_DefaultOutput];
    [mOutputNode retain];
    
    mEffectNode = nil;
	
    mConverterNode = [mGraph addNodeWithType: kAudioUnitType_FormatConverter
                                     subType: kAudioUnitSubType_AUConverter];
    [mConverterNode retain];
    
    mEffectEnabled = NO;
	[mGraph connectNode: mConverterNode output: 0
				 toNode: mOutputNode input: 0];
	
    //[self setIndexOfCurrentEffect: 0];
	
    [mGraph open];
    
    mConverterUnit = [[mConverterNode audioUnit] retain];
    [mConverterUnit setRenderCallback: MyRenderer context: self];
    
	AudioStreamBasicDescription streamFormat;
	streamFormat.mSampleRate = sampleRate;		//	the sample rate of the audio stream
	streamFormat.mFormatID = kAudioFormatLinearPCM;			//	the specific encoding type of audio stream
	streamFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger
		| kAudioFormatFlagsNativeEndian;// | kAudioFormatFlagIsNonMixable;
	
	
	
	streamFormat.mBytesPerPacket = 4;	
	streamFormat.mFramesPerPacket = 1;	
	streamFormat.mBytesPerFrame = 4;		
	streamFormat.mChannelsPerFrame = 2;	
	streamFormat.mBitsPerChannel = 16;	
	
    [mConverterUnit setStreamFormatWithDescription: &streamFormat];
    
    mBytesPerFrame = streamFormat.mBytesPerFrame;
	
    // Initialize unit
    [mGraph update];
    [mGraph initialize];
    
	
	
	[mGraph start];
	
	//CreateDefaultAU();
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



-(int)getWidth
{
	return cur_width;
}

-(int)getHeight
{
	return cur_height;
}

-(void)executeFrame
{
	double           currTime, deltaFPSTime, frameRate;
	static long      frameCount = 0;
	static double    lastFPSUpdate = 0.0;
	
	currTime = CurrentTime();
	frameCount++;
	deltaFPSTime = currTime - lastFPSUpdate;
	if (deltaFPSTime >= UPDATE_INTERVAL)
	{
		frameRate = (double)frameCount / deltaFPSTime;
		//	printf("FPS: %0.1f", frameRate);
		lastFPSUpdate = currTime;
		frameCount = 0;
	}		
	
	emulator.Execute(nstVideo,nstSound,&controls);
	
	[self drawRect:[self bounds]]; //Just draw the frame, allows drawing during resize
	
	//TestDefaultAU();
}

-(void)testBarcode
{
	Nes::Api::BarcodeReader br(emulator);
	char barcode[14];
	if(br.CanTransfer())
	{
		int len = br.Randomize(barcode);
		br.Transfer(barcode,len);
	}
}

-(void)startTimer:(float)fps
{
	[gameTimer release];
	gameTimer = [NSTimer timerWithTimeInterval:(1.0/fps)	// a 100ms time interval
										target:self                                      // Target is this object
									  selector:@selector(executeFrame)       // What function are we calling
									  userInfo:nil repeats:YES];                // No userinfo / repeat infinitely
	
	[[NSRunLoop currentRunLoop] addTimer:gameTimer
								 forMode:NSDefaultRunLoopMode];
	[[NSRunLoop currentRunLoop] addTimer:gameTimer
								 forMode:NSEventTrackingRunLoopMode];
}

void LoadGame(const char* filename)
{
	/*
	 //	char* buf[1000];
	 //	printf("Loading unzip\n");
	 //	printf("Loading zip - %d\n",unzOpen(filename));
	 //	unzFile testZip = unzOpen(filename);	
	 if(testZip == NULL)
	 {
		 printf("Couldn't open Zip!\n");
	 }
	 else 
	 {
		 printf("Opened zip with handle - %d\n",testZip);
	 }
	 printf("Going to first file - %d",unzGoToFirstFile(testZip));
	 printf("Reading first file - %d",unzReadCurrentFile(testZip,buf,1000));
	 */
	// acquire interface to machine
	Nes::Api::Machine machine( emulator );
	Nes::Api::Cartridge::Database database( emulator );
	
	FILE *f;
	int length;
	unsigned char *buffer;
	
	// this is a little ugly
	if (database.IsLoaded())
	{
		f = fopen(filename, "rb");
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		
		buffer = (unsigned char *)malloc(length);
		fread(buffer, length, 1, f);
		fclose(f);
		
		dbentry = database.FindEntry(buffer, length);
		
		free(buffer);
	}
	
	
	// C++ file stream
	std::ifstream file(filename , std::ios::in|std::ios::binary );
	
	// load game
	Nes::Result result = machine.Load( file );
	
	
	
	// failed?
	if (NES_FAILED(result))
	{
		switch (result)
		{
			case Nes::RESULT_ERR_INVALID_FILE:
				printf("Invalid file\n");
				break;
				
			case Nes::RESULT_ERR_OUT_OF_MEMORY:
				printf("Out of memory\n");
				break;
				
			case Nes::RESULT_ERR_CORRUPT_FILE:
				printf("Corrupt or missing file\n");
				break;
			case Nes::RESULT_ERR_UNSUPPORTED_MAPPER:
				printf("Unsupported mapper\n");
				break;
				
			case Nes::RESULT_ERR_MISSING_BIOS:
				printf("Can't find disksys.rom for FDS game\n");
				break;
			default:
				printf("Unknown error # %d \n",result);
				break;
		}
		return;
	}	
	
	// power on
	machine.Power( true ); // false = power off
}

-(void)loadDatabase
{
	Nes::Api::Cartridge::Database database( emulator );
	
	NSBundle* bundle = [NSBundle mainBundle];
	NSString* datFile = [bundle pathForResource:@"NstDatabase" ofType:@"dat"];
	
	std::ifstream *nstDBFile;
	
	nstDBFile = new std::ifstream([datFile UTF8String]/* cStringUsingEncoding:NSUTF8StringEncoding]*/, std::ifstream::in|std::ifstream::binary);
	
	if (nstDBFile->is_open())
	{
		database.Load(*nstDBFile);
		database.Enable(true);
	}
	else
		NSLog(@"OpenNestopia: Could not load Database file.");
	delete nstDBFile;
}

-(void)powerOff
{
	Nes::Api::Machine machine( emulator );
	machine.Power(false);// (true);
		[gameTimer invalidate];
		
}

-(void)resetGame
{
	Nes::Api::Machine machine( emulator );
	machine.Reset(true);
	
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
    if (pixelFormat == nil)
    {
        return nil;
    }
    [pixelFormat autorelease];
    
    MyAttributes[0] = NSOpenGLPFAFullScreen;
    NSOpenGLPixelFormat* fullScreenPixelFormat =
        [[NSOpenGLPixelFormat alloc] initWithAttributes:MyAttributes];
    if (fullScreenPixelFormat == nil)
    {
        return nil;
    }
    [fullScreenPixelFormat autorelease];
    
    self = [super initWithFrame:frameRect pixelFormat:pixelFormat];
    if (self == nil)
    {
        return nil;
    }
    
    fullScreenGLContext =
        [[NSOpenGLContext alloc] initWithFormat:fullScreenPixelFormat
                                   shareContext:[self openGLContext]];
    if (fullScreenGLContext == nil)
    {
        [self dealloc];
        return nil;
    }
    
    fullscreen = NO;
	//NSLog(@"init");
    return self;
}

-(IBAction)startLoad:(id)sender
{
	
	NSArray *fileTypes = [NSArray arrayWithObjects:@"nes",@"zip", nil]; 
	NSOpenPanel* oPanel = [NSOpenPanel openPanel]; 
	[oPanel setCanChooseDirectories:NO]; 
	[oPanel setCanChooseFiles:YES]; 
	[oPanel setCanCreateDirectories:NO]; 
	[oPanel setAllowsMultipleSelection:NO]; 
	[oPanel setAlphaValue:0.95]; 
	
	[oPanel setTitle:@"Nestopia: Load"];
	if ( [oPanel runModalForDirectory:nil file:nil types:fileTypes] == NSOKButton ){
		NSArray* files = [oPanel filenames];
		NSString* fileName = [files objectAtIndex:0]; 
		NSLog(fileName);
		saveName = [[[fileName lastPathComponent] stringByDeletingPathExtension] stringByAppendingPathExtension:@"sav"];
		[saveName retain];
		NSLog(saveName);
		LoadGame([fileName UTF8String]);// cStringUsingEncoding:NSUTF8StringEncoding]);
	}
	
	if(sender!=nil)
	{
		[mainWindow makeKeyAndOrderFront:nil];
		[self startTimer:60.0f];
	}
	
	
}
-(void)awakeFromNib
{	
	soundLock = [[NSLock alloc] init];
	void* userData = (void*) 0xDEADC0DE;
	
	// setup sound lock/unlock callbacks (for other sound related callbacks, see NstApiSound.hpp)
	Nes::Api::Sound::Output::lockCallback.Set( SoundLock, userData );
	Nes::Api::Sound::Output::unlockCallback.Set( SoundUnlock, userData );	
	Nes::Api::Video::Output::lockCallback.Set( VideoLock, userData );
	Nes::Api::Video::Output::unlockCallback.Set( VideoUnlock, userData );
	Nes::Api::User::fileIoCallback.Set( DoFileIO, userData );
	
	[self loadDatabase];
	
	playing = true;
	
	
	
	[self startLoad:nil];
	nstSound = new Nes::Api::Sound::Output; 
		
	sampleRate   = 44100;
	bufFrameSize = (sampleRate / 60);
	bufSize      = bufFrameSize*2;
	bufInPos     = 0;
	bufOutPos    = 0;
	bufUsed      = 0;
	
	nstSound->samples[0] = soundBuffer;
	nstSound->length[0] = bufFrameSize;
	nstSound->samples[1] = NULL;
	nstSound->length[1] = 0;
	
memset(soundBuffer, 0, sizeof(soundBuffer));
	
	
	[self setupVideo];
	[self setupInput];
	[self setupSound];
	
	[self setupGL];
	//[oglContext setFullScreen];
	[self resetVideo];
	
	
	NSRect oldFrame = [mainWindow frame];
	NSRect newFrame = [self frame];
	newFrame.origin = oldFrame.origin;
	NSSize aspect;
	switch([[NSUserDefaults standardUserDefaults] integerForKey:@"aspectCorrection"])
	{
		case 0: //None
			aspect = NSMakeSize(cur_width,cur_height);
			
			break;
		case 1: //4:3
			aspect = NSMakeSize(4,3);
			break;
		case 2: //16:9
			aspect = NSMakeSize(16,9);
	}
	
	newFrame.size = NSMakeSize(cur_width,cur_height);
	
	[mainWindow setContentAspectRatio:aspect];
	
	[mainWindow setContentSize:newFrame.size];	
	[mainWindow makeKeyAndOrderFront:nil];
	
	//TestDefaultAU();
	
	[self startTimer:60.0f];
}

-(void)setCode:(NSString*)code
{
	if(![code isEqualToString:@""])
	{
		char cCode[9];
		[code getCString:cCode];
		Nes::Api::Cheats cheater(emulator);
		Nes::Api::Cheats::Code ggCode;
		Nes::Api::Cheats::GameGenieDecode(cCode, ggCode);
		cheater.SetCode(ggCode);
	}
	else
	{
		NSLog(@"Null code");
	}
}

-(void)setFrame:(NSRect)frame
{
	[super setFrame:frame];
	[self resetVideo];
}

-(void)setupGL
{
	
	GLuint textures;
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_DITHER);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	glHint(GL_TRANSFORM_HINT_APPLE, GL_FASTEST);
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	glGenTextures(1, &textures);
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 1);
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, 1);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGB5, cur_width, cur_height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, videoScreen);
	glClearColor (0.0, 0.0, 1.0, 1.0);
	glClearDepth(1.0);
	
	fGLSetup = true;
}

- (void) ddhidKeyboard: (DDHidKeyboard *) keyboard
				 keyUp: (unsigned) usageId;
{
	if([mainWindow isKeyWindow])
	{
		switch(usageId)
		{
			
			case 82:
				controls.pad[0].buttons ^= Nes::Api::Input::Controllers::Pad::UP;
				break;
			case 79:
				controls.pad[0].buttons ^= Nes::Api::Input::Controllers::Pad::RIGHT;
				break;
			case 81:
				controls.pad[0].buttons ^= Nes::Api::Input::Controllers::Pad::DOWN;
				break;
			case 80:
				controls.pad[0].buttons ^= Nes::Api::Input::Controllers::Pad::LEFT;
				break;
			case 4:
				controls.pad[0].buttons ^= Nes::Api::Input::Controllers::Pad::A;
				break;
			case 22:
				controls.pad[0].buttons ^= Nes::Api::Input::Controllers::Pad::B;
				break;
			case 40:
				controls.pad[0].buttons ^= Nes::Api::Input::Controllers::Pad::START;
				break;	
			case 49:
				controls.pad[0].buttons ^= Nes::Api::Input::Controllers::Pad::SELECT;
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
				controls.pad[0].buttons |= Nes::Api::Input::Controllers::Pad::UP;
				break;
			case 79:
				controls.pad[0].buttons |= Nes::Api::Input::Controllers::Pad::RIGHT;
				break;
			case 81:
				controls.pad[0].buttons |= Nes::Api::Input::Controllers::Pad::DOWN;
				break;
			case 80:
				controls.pad[0].buttons |= Nes::Api::Input::Controllers::Pad::LEFT;
				break;
			case 4:
				controls.pad[0].buttons |= Nes::Api::Input::Controllers::Pad::A;
				break;
			case 22:
				controls.pad[0].buttons |= Nes::Api::Input::Controllers::Pad::B;
				break;
			case 40:
				controls.pad[0].buttons |= Nes::Api::Input::Controllers::Pad::START;
				break;	
			case 49:
				controls.pad[0].buttons |= Nes::Api::Input::Controllers::Pad::SELECT;
				break;
		}
	}
}


- (IBAction)fullscreenToggle:(id)sender
{
	if(!fullscreen)
	{
		CGCaptureAllDisplays();
		[fullScreenGLContext setFullScreen];
		[fullScreenGLContext makeCurrentContext];
		fullscreen = YES;
		
		[self setupGL];
		[self resetVideo];
		
		[NSMenu setMenuBarVisible:NO];
		[NSCursor hide];
	}
	
	else
	{
		[NSCursor unhide];
		
		[NSMenu setMenuBarVisible:YES];
		[fullScreenGLContext clearDrawable];
		[[self openGLContext] makeCurrentContext];
		CGReleaseAllDisplays();
		fullscreen = NO;
		
		[self setupGL];
		[self resetVideo];
	}
    
}


- (void)mouseMoved:(NSEvent *)theEvent
{
	NSPoint mousep;
	NSRect rect = [self frame];
	mousep = [self convertPoint: [theEvent locationInWindow]  fromView: nil];
	controls.paddle.x =  (256.0 / (double)(rect.size.width)) * mousep.x;
}

- (void)mouseDown:(NSEvent *)theEvent 
{
	//NSLog(@"Clicky!");
	//get the mouse position in view coordinates
	if([theEvent modifierFlags] & NSShiftKeyMask)
	{
		controls.zapper.x = 0;
		controls.zapper.y = 0;
	}
	else
	{
		NSPoint mousep;
		NSRect rect = [self bounds];
		mousep = [self convertPoint: [theEvent locationInWindow]  fromView: nil];
		controls.zapper.x =  (256.0 / NSWidth(rect)) * mousep.x;
		controls.zapper.y = abs(((240.0 / NSHeight(rect)) * mousep.y)-240);
	}
	controls.paddle.button = true;
	controls.zapper.fire = true;
}

-(void)mouseUp:(NSEvent *)theEvent 
{
	controls.paddle.button = false;
	controls.zapper.fire = false;
}

// this is called whenever the view changes (is unhidden or resized)
- (void)drawRect:(NSRect)frameRect 
{
	
    glClear(GL_COLOR_BUFFER_BIT);
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT,1);// (screen1?2:1));
		glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, 1);
		
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, 0, 0, cur_width, cur_height, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV,videoScreen);//(screen1?videoScreen2:videoScreen1));
			NSRect f;
			if(!fullscreen)
			{
				f = [self bounds];
				glBegin(GL_QUADS);         
				{
					glTexCoord2f(0, 0);
					glVertex2f(0, 0);
					glTexCoord2f(0, cur_height);
					glVertex2f(0, NSHeight(f));
					glTexCoord2f(cur_width, cur_height);
					glVertex2f(NSWidth(f), NSHeight(f));
					glTexCoord2f(cur_width, 0);
					glVertex2f(NSWidth(f), 0);
				}
				glEnd();
			}
			else
			{
				f=[[NSScreen mainScreen] frame];
				float h_ratio = NSHeight(f)/(float)cur_height;
				
				f=NSMakeRect((NSWidth(f)-(cur_width*h_ratio))/2,0,cur_width * h_ratio,cur_height*h_ratio);
				glBegin(GL_QUADS);         
				{
					glTexCoord2f(0, 0);
					glVertex2f(NSMinX(f), NSMinY(f));
					glTexCoord2f(0, cur_height);
					glVertex2f(NSMinX(f), NSMaxY(f));
					glTexCoord2f(cur_width, cur_height);
					glVertex2f(NSMaxX(f), NSMaxY(f));
					glTexCoord2f(cur_width, 0);
					glVertex2f(NSMaxX(f), NSMinY(f));
				}
				
				glEnd();
			}
			
			//glFlush();
			[[NSOpenGLContext currentContext] flushBuffer];
}

-(void) dealloc 
{
	[super dealloc];
	delete videoScreen;
}

-(void) loadState
{
	Nes::Api::Machine machine( emulator );	
	NSArray *fileTypes = [NSArray arrayWithObjects:@"sav", nil]; 
	NSOpenPanel* oPanel = [NSOpenPanel openPanel]; 
	[oPanel setCanChooseDirectories:NO]; 
	[oPanel setCanChooseFiles:YES]; 
	[oPanel setCanCreateDirectories:NO]; 
	[oPanel setAllowsMultipleSelection:NO]; 
	[oPanel setAlphaValue:0.95]; 
	
	[oPanel setTitle:@"Nestopia: Load State"];
	if ( [oPanel runModalForDirectory:nil file:nil types:fileTypes] == NSOKButton ){
		NSArray* files = [oPanel filenames];
		NSString* fileName = [files objectAtIndex:0]; 
		NSLog(fileName);
		const char* filename = [fileName UTF8String];// cStringUsingEncoding:NSUTF8StringEncoding];
			std::ifstream stateFile( filename, std::ifstream::in|std::ifstream::binary );
		
		if (stateFile.is_open())
		{
			machine.LoadState(stateFile);
		}
		
	}	
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

-(void) saveState
{
	NSSavePanel* sPanel = [NSSavePanel savePanel];
	[sPanel setCanCreateDirectories:YES];
	[sPanel setAlphaValue:0.95];
	[sPanel setTitle:@"OpenNestopia: Save State"];
	//	[sPanel filename:saveName];
	if( [sPanel runModalForDirectory:nil file:saveName] == NSOKButton)
	{
		Nes::Api::Machine machine( emulator );
		
		char defname[512];
		
		defname[0] = '\0';
		//	strcpy(defname, rootname);
		//		strcat(defname, ".nst");
		
		NSString* files = [sPanel filename];
		
		const char *filename = [files UTF8String];// cStringUsingEncoding:NSUTF8StringEncoding];
			
			std::ofstream stateFile( filename, std::ifstream::out|std::ifstream::binary );
		
		if (stateFile.is_open())
		{
			machine.SaveState(stateFile);
		}
		
	}
}

- (void)keyDown:(NSEvent *)theEvent 
{
	
}

#pragma mark --Emulation Callbacks--
// called right before Nestopia is about to write pixels
static bool NST_CALLBACK VideoLock(void* userData,Nes::Api::Video::Output& video)
{
	video.pixels = videoScreen;//(screen1?videoScreen1:videoScreen2);
	video.pitch = cur_width*2;
	return true; // true=lock success, false=lock failed (Nestopia will carry on but skip video)
}

// called right after Nestopia has finished writing pixels (not called if previous lock failed)
static void NST_CALLBACK VideoUnlock(void* userData,Nes::Api::Video::Output& video)
{
	video.pixels = NULL;
	//screen1 = screen1?false:true;
}

// for various file operations, usually called during image file load, power on/off and reset
static void NST_CALLBACK DoFileIO(void* userData,Nes::Api::User::File operation,Nes::Api::User::FileData& data)
{
	switch (operation)
	{
		case Nes::Api::User::FILE_LOAD_BATTERY: // load in battery data from a file
		case Nes::Api::User::FILE_LOAD_EEPROM: // used by some Bandai games, can be treated the same as battery files
		{
			NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
			
			NSString *rootSupportPath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Library"]
															stringByAppendingPathComponent:@"Application Support"];
			
			NSString *appSupportPath = [rootSupportPath stringByAppendingPathComponent:@"OpenNestopia"];
			NSString *batSavePath = [appSupportPath stringByAppendingPathComponent:@"Battery Saves"];
			NSString *batSaveFile = [batSavePath stringByAppendingPathComponent:saveName];
			
			NSData* fileData = [NSData dataWithContentsOfFile:batSaveFile];
			
			data.resize( [fileData length] );
			
			
			memcpy(&data.front(),[fileData bytes],[fileData length]);			
			[pool release];
			
			break;
		}
			
		case Nes::Api::User::FILE_SAVE_BATTERY: // save battery data to a file
		case Nes::Api::User::FILE_SAVE_EEPROM: // can be treated the same as battery files
		{
			
			NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
			
			NSString *rootSupportPath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Library"]
															stringByAppendingPathComponent:@"Application Support"];
			
			NSString *appSupportPath = [rootSupportPath stringByAppendingPathComponent:@"OpenNestopia"];
			NSString *batSavePath = [appSupportPath stringByAppendingPathComponent:@"Battery Saves"];
			NSString *batSaveFile = [batSavePath stringByAppendingPathComponent:saveName];
			
			NSData* fileData = [NSData dataWithBytes:(const char*)&data.front() length:data.size()];
			
			NSFileManager *fileManager = [NSFileManager defaultManager];
			
			[fileManager createDirectoryAtPath:appSupportPath attributes:nil];
			[fileManager createDirectoryAtPath:batSavePath attributes:nil];
			[fileManager createFileAtPath:batSaveFile contents:fileData attributes:nil];
			
			[pool release];	
			break;
		}
			
		case Nes::Api::User::FILE_SAVE_FDS: // for saving modified Famicom Disk System files
		{
			/*		char fdsname[512];
			
			sprintf(fdsname, "%s.fds", savename);
			
			std::ofstream fdsFile( fdsname, std::ifstream::out|std::ifstream::binary );
			
			if (fdsFile.is_open())
			fdsFile.write( (const char*) &data.front(), data.size() );
			
			break;*/
		}
			
		case Nes::Api::User::FILE_LOAD_TAPE: // for loading Famicom cassette tapes
		case Nes::Api::User::FILE_SAVE_TAPE: // for saving Famicom cassette tapes
		case Nes::Api::User::FILE_LOAD_TURBOFILE: // for loading turbofile data
		case Nes::Api::User::FILE_SAVE_TURBOFILE: // for saving turbofile data
			break;
	}
}

// called right before Nestopia is about to write sound samples
static bool NST_CALLBACK SoundLock(void* userData,Nes::Api::Sound::Output& sound)
{
	if([soundLock tryLock])
		return true;
	return false;
}

static void NST_CALLBACK SoundUnlock(void* userData,Nes::Api::Sound::Output& sound)
{
	bufInPos = (bufInPos + bufFrameSize) % bufSize;
	bufUsed += bufFrameSize;
	if (bufUsed > bufSize)
	{
		bufUsed   = bufSize;
		bufOutPos = bufInPos;
	}
	nstSound->samples[0] = &soundBuffer[bufInPos];
	[soundLock unlock];
}


void	CreateDefaultAU()
{
	OSStatus err = noErr;
	
	// Open the default output unit
	ComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_DefaultOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	
	Component comp = FindNextComponent(NULL, &desc);
	if (comp == NULL) { printf ("FindNextComponent\n"); return; }
	
	err = OpenAComponent(comp, &gOutputUnit);
	if (comp == NULL) { printf ("OpenAComponent=%ld\n", err); return; }
	
	// Set up a callback function to generate output to the output unit
    AURenderCallbackStruct input;
	input.inputProc = MyRenderer;
	input.inputProcRefCon = NULL;
	
	err = AudioUnitSetProperty (gOutputUnit, 
								kAudioUnitProperty_SetRenderCallback, 
								kAudioUnitScope_Input,
								0, 
								&input, 
								sizeof(input));
	
	if (err) { printf ("AudioUnitSetProperty-CB=%ld\n", err); return; }
    
}

void	TestDefaultAU()
{
	OSStatus err = noErr;
    
	// We tell the Output Unit what format we're going to supply data to it
	// this is necessary if you're providing data through an input callback
	// AND you want the DefaultOutputUnit to do any format conversions
	// necessary from your format to the device's format.
	AudioStreamBasicDescription streamFormat;
	
	streamFormat.mSampleRate = sampleRate;		//	the sample rate of the audio stream
	streamFormat.mFormatID = kAudioFormatLinearPCM;			//	the specific encoding type of audio stream
	streamFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger
		| kAudioFormatFlagsNativeEndian
		| kAudioFormatFlagIsPacked;
	
	
	
	streamFormat.mBytesPerPacket = 4;	
	streamFormat.mFramesPerPacket = 1;	
	streamFormat.mBytesPerFrame = 4;		
	streamFormat.mChannelsPerFrame = 2;	
	streamFormat.mBitsPerChannel = 16;	
	
	err = AudioUnitSetProperty (gOutputUnit,
								kAudioUnitProperty_StreamFormat,
								kAudioUnitScope_Input,
								0,
								&streamFormat,
								sizeof(AudioStreamBasicDescription));
	
	if (err) { printf ("AudioUnitSetProperty-SF=%4.4s, %ld\n", (char*)&err, err); return; }
	
    // Initialize unit
	err = AudioUnitInitialize(gOutputUnit);
	if (err) { printf ("AudioUnitInitialize=%ld\n", err); return; }
    
	Float64 outSampleRate;
	UInt32 size = sizeof(Float64);
	err = AudioUnitGetProperty (gOutputUnit,
								kAudioUnitProperty_SampleRate,
								kAudioUnitScope_Output,
								0,
								&outSampleRate,
								&size);
	
	//printf("Out srate %f\n",outSampleRate);
	if (err) { printf ("AudioUnitSetProperty-GF=%4.4s, %ld\n", (char*)&err, err); return; }
	AudioOutputUnitStart (gOutputUnit);
}

#pragma mark --Dip Switches--
-(int)numOfDips
{
	Nes::Api::DipSwitches dips(emulator);
	return dips.NumDips();
}

-(NSString*)getDipName:(int)num
{	
	Nes::Api::DipSwitches dips(emulator);
	return [NSString stringWithCString:dips.GetDipName(num)];
}

-(NSArray*)getDipValues:(int)num
{
	int i;
	Nes::Api::DipSwitches dips(emulator);
	NSString* strings[dips.NumValues(num)];
	for(i = 0;i<dips.NumValues(num);i++)
	{
		strings[i] = [NSString stringWithCString:dips.GetValueName(num,i)];
	}
	return [NSArray arrayWithObjects:strings count:dips.NumValues(num)];
}

-(void)setDipValuesForDip:(int)dip value:(int)value
{
	Nes::Api::DipSwitches dips(emulator);
	dips.SetValue(dip,value);
}

-(int)getSelectedValue:(int)num
{
	Nes::Api::DipSwitches dips(emulator);
	return dips.GetValue(num);
}

@end
