#import <Cordova/CDV.h>

@interface HelloCPlugin : CDVPlugin

- (void) hello:(CDVInvokedUrlCommand*)command;
- (void) getArch:(CDVInvokedUrlCommand*)command;
- (void) calculate:(CDVInvokedUrlCommand*)command;

@end
