#import "OpenNestopiaController.h"
#import "OpenNestopiaPreferences.h"
#import "DebuggerWindowController.h"


@implementation OpenNestopiaController

- (void)windowWillMiniaturize:(NSNotification *)aNotification 
{ 
    [nesView copyGLToBackingStore]; 
} 

- (void)windowWillClose:(NSNotification *)aNotification
{
	[nesView powerOff];
}

- (IBAction)applyCheats:(id)sender
{
	NSArray* codes = [gameSharkCode cells];

	for(int i = 0; i < [codes count]; i ++) {
        //
	}
}

-(IBAction)buttonChanged:(id)sender
{
	NSInteger row;
	
	[sender getNumberOfRows:&row columns:nil];
	
	for (NSInteger i = 0; i<row; i++) {
		[sender cellAtRow:i column:1];
		// NSPopUpButtonCell* temp = [sender cellAtRow:i column:1];
        // 
	}
}

-(IBAction)testDips:(id)sender
{	
//	if([nestopiaView numOfDips]>0)
//	{
//		NSTextFieldCell* tempCell;
//		NSPopUpButtonCell* popupCell;
//		NSArray* test;
//		
//		int i;
//		int numOfRows = [dipSwitchLabels numberOfRows];
//		for(i = 0;i<numOfRows;i++)
//		{
//			[dipSwitchLabels removeRow:0];
//		}
//		
//		for(i = 0;i<[nestopiaView numOfDips];i++)
//		{
//			tempCell = [[NSTextFieldCell alloc] initTextCell: [nestopiaView getDipName:i]];
//			//[tempStr release];
//			popupCell = [[NSPopUpButtonCell alloc] init];
//			[popupCell setPullsDown:false];
//			[popupCell setTarget:self];
//			[popupCell setAction:@selector(buttonChanged:)];
//		
//			[popupCell  addItemsWithTitles:[nestopiaView getDipValues:i]];
//			[popupCell selectItemAtIndex:[nestopiaView getSelectedValue:i]];
//			[popupCell setEnabled:YES];	
//			
//			test = [NSArray arrayWithObjects:tempCell,popupCell, nil];
//			[dipSwitchLabels addRowWithCells:test];
//		}
//		
//		[dipSwitchLabels sizeToCells];
//		[dipSwitchLabels sizeToFit];
//		NSRect labelFrame = [dipSwitchLabels frame];
//		[dipSwitchPanel setContentSize:NSMakeSize(labelFrame.size.width+40,labelFrame.size.height+40)];
//		[dipSwitchLabels setFrameOrigin:NSMakePoint(20,20)];
//		[dipSwitchPanel makeKeyAndOrderFront:nil];
//	}
//	else
//	{
//		NSAlert *alert = [[NSAlert alloc] init];
//		[alert addButtonWithTitle:@"Ok"];
//		[alert setMessageText:@"This game does not have any DIP switches."];
//		[alert setAlertStyle:NSWarningAlertStyle];
//		[alert beginSheetModalForWindow:mainWindow modalDelegate:nil didEndSelector:nil contextInfo:nil];
//	}
}


-(void)setupToolbar
{
	items=[[NSMutableDictionary alloc] init];
	
	NSString *name;
	NSToolbarItem *item;
	
	name=[[NSString alloc] initWithFormat:@"Play"];
	item=[[NSToolbarItem alloc] initWithItemIdentifier:name];
	[item setPaletteLabel:name]; // name for the "Customize Toolbar" sheet
	[item setLabel:name]; // name for the item in the toolbar
	[item setToolTip:[NSString stringWithFormat:@"This is the play button"]]; // tooltip
	[item setTarget:self]; // what should happen when it's clicked
	[item setAction:@selector(toolbaritemclicked:)];
	[item setImage:[NSImage imageNamed:name]];
	[item setEnabled:YES];
	[items setObject:item forKey:name]; // add to toolbar list
	
	name=[[NSString alloc] initWithFormat:@"Pause"];
	item=[[NSToolbarItem alloc] initWithItemIdentifier:name];
	[item setPaletteLabel:name]; // name for the "Customize Toolbar" sheet
	[item setLabel:name]; // name for the item in the toolbar
	[item setToolTip:[NSString stringWithFormat:@"This is the pause button"]]; // tooltip
	[item setTarget:self]; // what should happen when it's clicked
	[item setAction:@selector(toolbaritemclicked:)];
	[item setImage:[NSImage imageNamed:name]];
	[item setEnabled:YES];
	[items setObject:item forKey:name]; // add to toolbar list
	
	name=[[NSString alloc] initWithFormat:@"Stop"];
	item=[[NSToolbarItem alloc] initWithItemIdentifier:name];
	[item setPaletteLabel:name]; // name for the "Customize Toolbar" sheet
	[item setLabel:name]; // name for the item in the toolbar
	[item setToolTip:[NSString stringWithFormat:@"This is the stop button"]]; // tooltip
	[item setTarget:self]; // what should happen when it's clicked
	[item setAction:@selector(toolbaritemclicked:)];
	[item setImage:[NSImage imageNamed:name]];
	[item setEnabled:YES];
	[items setObject:item forKey:name]; // add to toolbar list	
	
	name=[[NSString alloc] initWithFormat:@"Reset"];
	item=[[NSToolbarItem alloc] initWithItemIdentifier:name];
	[item setPaletteLabel:name]; // name for the "Customize Toolbar" sheet
	[item setLabel:name]; // name for the item in the toolbar
	[item setToolTip:[NSString stringWithFormat:@"This is the reset button"]]; // tooltip
	[item setTarget:nesView]; // what should happen when it's clicked
	[item setAction:@selector(resetGame)];
	[item setImage:[NSImage imageNamed:name]];
	[item setEnabled:YES];
	[items setObject:item forKey:name]; // add to toolbar list
	
	name=[[NSString alloc] initWithFormat:@"Full Screen"];
	item=[[NSToolbarItem alloc] initWithItemIdentifier:name];
	[item setPaletteLabel:name]; // name for the "Customize Toolbar" sheet
	[item setLabel:name]; // name for the item in the toolbar
	[item setToolTip:[NSString stringWithFormat:@"Switch to fullscreen"]]; // tooltip
	[item setTarget:nesView]; // what should happen when it's clicked
	[item setAction:@selector(fullscreenToggle:)];
	[item setImage:[NSImage imageNamed:name]];
	[item setEnabled:YES];
	[items setObject:item forKey:name]; // add to toolbar list
	
	name=[[NSString alloc] initWithFormat:@"Save State"];
	item=[[NSToolbarItem alloc] initWithItemIdentifier:name];
	[item setPaletteLabel:name]; // name for the "Customize Toolbar" sheet
	[item setLabel:name]; // name for the item in the toolbar
	[item setToolTip:[NSString stringWithFormat:@"This is the reset button"]]; // tooltip
	[item setTarget:nesView]; // what should happen when it's clicked
	[item setAction:@selector(saveState)];
	[item setImage:[NSImage imageNamed:name]];
	[item setEnabled:YES];
	[items setObject:item forKey:name]; // add to toolbar list
	
	name=[[NSString alloc] initWithFormat:@"Load State"];
	item=[[NSToolbarItem alloc] initWithItemIdentifier:name];
	[item setPaletteLabel:name]; // name for the "Customize Toolbar" sheet
	[item setLabel:name]; // name for the item in the toolbar
	[item setToolTip:[NSString stringWithFormat:@"This is the reset button"]]; // tooltip
	[item setTarget:nesView]; // what should happen when it's clicked
	[item setAction:@selector(loadState)];
	[item setImage:[NSImage imageNamed:name]];
	[item setEnabled:YES];
	[items setObject:item forKey:name]; // add to toolbar list
	
	name=[[NSString alloc] initWithFormat:@"DIP Switches"];
	item=[[NSToolbarItem alloc] initWithItemIdentifier:name];
	[item setPaletteLabel:name]; // name for the "Customize Toolbar" sheet
	[item setLabel:name]; // name for the item in the toolbar
	[item setToolTip:[NSString stringWithFormat:@"This is the reset button"]]; // tooltip
	[item setTarget:self]; // what should happen when it's clicked
	[item setAction:@selector(testDips:)];
	[item setImage:[NSImage imageNamed:name]];
	[item setEnabled:YES];
	[items setObject:item forKey:name]; // add to toolbar list
	
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"tooltest"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	[mainWindow setToolbar:toolbar];
	
}

-(void)awakeFromNib
{   	
	[self setupToolbar];
}

- (IBAction)openPreferencesWindow:(id)sender
{	
	[[OpenNestopiaPreferences sharedPrefsWindowController] showWindow:nil];
}

- (IBAction)openDebugConsole:(id)sender
{
    DebuggerWindowController *debuggerWindowController = [[DebuggerWindowController alloc] initWithEmu:nesView.emu];
    if (debuggerWindowController) {
        [debuggerWindowController showWindow:self];
    }
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
	 itemForItemIdentifier:(NSString *)itemIdentifier
		willBeInsertedIntoToolbar:(BOOL)flag 
{
    return [items objectForKey:itemIdentifier];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar
{
    return [items allKeys];
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar
{
    return [items allKeys];// subarrayWithRange:NSMakeRange(0,3)];
}

- (void) toolbarWillAddItem: (NSNotification *) notification
{
//    NSToolbarItem *addedItem = [[notification userInfo] objectForKey:@"item"];
//	[addedItem 
    // set up the item here
}

- (void)toolbarDidRemoveItem:(NSNotification *)notification
{
  //  NSToolbarItem *addedItem = [[notification userInfo] objectForKey:@"item"];
	
    // clear associated info here 
}


@end
