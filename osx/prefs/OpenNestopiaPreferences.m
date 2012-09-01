//
//  OpenNestopiaPreferences.m
//  OpenNestopia
//
//  Created by Joshua Weinberg on 6/13/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "OpenNestopiaPreferences.h"


@implementation OpenNestopiaPreferences


- (void)setupToolbar
{
	[self addView:generalPrefsView label:@"General"];
	[self addView:videoPrefsView label:@"Video"];
	[self addView:audioPrefsView label:@"Audio"];	
	[self addView:controlsPrefsView label:@"Controls"];
}


- (void)windowWillClose:(NSNotification *)aNotification
{
	//NSLog(@"Closed prefs");
}
@end
