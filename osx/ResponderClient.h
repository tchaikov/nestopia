//
//  ResponderClient.h
//  OpenNestopia
//
//  Created by Kefu Chai on 02/09/12.
//
//

#ifndef OpenNestopia_ResponderClient_h
#define OpenNestopia_ResponderClient_h

typedef enum _NESButton
{
    NESButtonUp,
    NESButtonDown,
    NESButtonLeft,
    NESButtonRight,
    NESButtonA,
    NESButtonB,
    NESButtonStart,
    NESButtonSelect,
    NESButtonCount
} NESButton;

@protocol NESSystemResponderClient <NSObject>

- (oneway void)didPushNESButton:(NESButton)button forPlayer:(NSUInteger)player;
- (oneway void)didReleaseNESButton:(NESButton)button forPlayer:(NSUInteger)player;

@end

#endif
