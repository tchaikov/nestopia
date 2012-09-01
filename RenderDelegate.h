//
//  RenderDelegate.h
//  OpenNestopia
//
//  Created by Kefu Chai on 02/09/12.
//
//

#import <Foundation/Foundation.h>

@protocol RenderDelegate

@required
- (void) willExecute;
- (void) didExecute;
@end
