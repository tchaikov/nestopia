//
//  util.h
//  OpenNestopia
//
//  Created by Kefu Chai on 01/09/12.
//
//

#ifndef OpenNestopia_util_h
#define OpenNestopia_util_h

#import <Foundation/Foundation.h>

#ifndef DLog

#  ifdef DEBUG_PRINT
#    define DLog(format, ...) NSLog(@"%s: " format, __FUNCTION__, ##__VA_ARGS__)
#  else
#    define DLog(format, ...) do {} while(0)
#  endif

#endif

#ifndef BOOL_STR
#  define BOOL_STR(b) ((b) ? "YES" : "NO")
#endif

typedef struct _IntSize {
    int width;
    int height;
} IntSize;

static inline IntSize SizeMake(int width, int height)
{
    return (IntSize){width, height};
}

#endif
