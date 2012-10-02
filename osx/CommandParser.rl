#import "CommandParser.h"

#import "Breakpoint.h"

%%{
  machine cmd;
  variable cs _currentState;
  variable ts _tokenStart;
  variable te _tokenEnd;
  variable act _act;
  # common
  action mark_begin {
    mark = fpc;
  }
  action save_dec {
    NSString *decString =
      [[NSString alloc] initWithBytes:mark
                               length:fpc - mark
                             encoding:NSUTF8StringEncoding];
    saved_dec = [decString intValue];
  }
  action save_hex {
    unsigned res;
    NSString *hexString =
      [[NSString alloc] initWithBytes:mark
                               length:fpc - mark
                             encoding:NSUTF8StringEncoding];
    [[NSScanner scannerWithString:hexString] scanHexInt:&res];
    saved_hex = res;
  }
  decimal = digit+ >mark_begin %save_dec;
  hex16 = xdigit{4} >mark_begin %save_hex;
  hex8  = xdigit{2} >mark_begin %save_hex;
  imm_addr = '$'hex16 %{saved_address = saved_hex;};
  address = imm_addr;
  sp = ' ';
  # inspect
  action print_var {
    [self.commandRunner printVar:saved_hex];
  }
  print_var = ('p' | 'print' ) sp+ address @print_var;

  action set {
    [self.commandRunner set:saved_address withValue:saved_value];
  }
  value = ( decimal %save_dec %{saved_value = saved_dec;} |
            hex8 %save_hex %{saved_value = saved_hex;} );
  set = 'set' ' '+ address ' '* '=' ' '* value @set;

  # breakpoint
  action breakp {
    _bp.address = saved_hex;
    [self.commandRunner setBreakpoint:_bp];
  }
  access_read = ('r' | 'read') %{ _bp.access = AccessRead;};
  access_write = ('w' | 'write') %{ _bp.access = AccessWrite;};
  access_rw = 'rw' %{ _bp.access = AccessRW;};
  access_mode = (access_read | access_write | access_rw);
  breakp = ('b' | 'break') sp (address | 'if' sp access_mode sp address) @breakp;

  action remove_bp {
    [self.commandRunner removeBreakpoint:dec];
  }
  delete = ('d' | 'del') sp+ decimal @remove_bp;

  action disable_bp {
    [self.commandRunner disableBreakpoint:dec];
  }
  disable = 'disable' sp+ decimal @disable_bp;

  action enable_bp {
    [self.commandRunner enableBreakpoint:dec];
  }
  enable = 'enable' sp+ decimal @enable_bp;

  # run
  action next {
    [self.commandRunner next];
  }
  next = ('n' | 'next') @next;
  action step {
    [self.commandRunner stepIn];
  }
  step = ('s' | 'step') @step;
  action until {
    [self.commandRunner until];
  }
  until = 'until' @until;

  # display
  action display {
    [self.commandRunner display:saved_expr];
  }
  action save_expr {
    saved_expr =
      [[NSString alloc] initWithBytes:mark
                               length:fpc - mark
                             encoding:NSUTF8StringEncoding];
  }

  var = ('$'xdigit+ | '%'ascii+) >mark_begin %save_expr;
  display = 'display' sp+ var @display;
  action undisplay {
    [self.commandRunner undisplay:saved_dec];
  }
  undisplay = 'undisplay' sp+ decimal @undisplay;

  # memory
  action search {
    [self.commandRunner searchBytes:_bytes];
  }
  action save_bytes {
    _bytes = [NSData dataWithBytes:mark
                            length:fpc - mark];
  }
  bytes = (xdigit{2})+ >mark_begin %save_bytes;
  search = 'search' sp+ bytes @search;

  # repeat
  action repeat {
    [self.commandRunner repeatLastCommand];
  }

  main := (empty @repeat |
           print_var |
           set |
           breakp |
           next |
           until |
           step |
           display |
           undisplay |
           search);
}%%

%% write data;

@implementation CommandParser

- (id)initWithRunner:(id<CommandRunner>)runner {
  if ((self = [super init]) != nil) {
    self.commandRunner = runner;
  }
  return self;
}

- (void)parse:(NSString *)command {
  const char *eof = 0;
  const char *p = [command UTF8String];
  const char *pe = p + [command lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
  const char *mark = 0;
  int saved_hex = ~0, saved_dec = ~0;
  uint16_t saved_address = ~0;
  uint8_t saved_value = ~0;
  NSString *saved_expr = nil;
  %% write init;
  %% write exec;
}

- (BOOL)completed {
  return _currentState >= cmd_first_final;
}

- (BOOL)error {
  return _currentState == cmd_error;
}

@end
