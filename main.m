//
//  main.m
//  OpenNestopia
//
//  Created by Joshua Weinberg on 7/6/07.
//  Copyright __MyCompanyName__ 2007. All rights reserved.
//

#import <Cocoa/Cocoa.h>

int main(int argc, char *argv[])
{
	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	//NSArray *testKeys = [[NSArray alloc] initWithObjects:@"Pad 1 Up",@"Pad 1 Left",@"Pad 1 Down",@"Pad 1 Right",nil];
	//NSArray *testVals = [[NSArray alloc] initWithObjects:@"Up",@"Left",@"Down",@"Right",nil];
	NSArray *pad1Keys = [[NSArray alloc]  initWithObjects:@"Up",@"Left",@"Down",@"Right",nil];
	NSDictionary *controlDefaults = [NSDictionary dictionaryWithObject:pad1Keys forKey:@"pad1"];
	[userDefaults registerDefaults:controlDefaults];
	[pool release];
	
    return NSApplicationMain(argc,  (const char **) argv);
}
