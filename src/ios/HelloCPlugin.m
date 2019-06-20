#import "HelloCPlugin.h"
#include "c_getArch.h"
#include "hello.h"
#include "mylib.h"
@implementation HelloCPlugin

- (void)getArch:(CDVInvokedUrlCommand*)command
{
    NSString* msg = [NSString stringWithFormat: @"iOS %s", getCPUArch()];
    
    CDVPluginResult* result = [CDVPluginResult
                               resultWithStatus:CDVCommandStatus_OK
                               messageAsString:msg];
    
    [self.commandDelegate sendPluginResult:result callbackId:command.callbackId];
}

- (void)hello:(CDVInvokedUrlCommand*)command
{
    //c_hello();
    NSString* input = [[command arguments] objectAtIndex:0];
    char* c_input = strdup([input UTF8String]);
    NSString* output = [NSString stringWithFormat: @"iOS says: %s", c_hello(c_input)];
    
    CDVPluginResult* result = [CDVPluginResult
                               resultWithStatus:CDVCommandStatus_OK
                               messageAsString:output];
    
    [self.commandDelegate sendPluginResult:result callbackId:command.callbackId];
}

- (void)calculate:(CDVInvokedUrlCommand *)command
{
    NSString* a = [[command arguments] objectAtIndex:0];
    NSString* b = [[command arguments] objectAtIndex:1];
    
    int x = a.intValue;
    int y = b.intValue;
    int output = calculate(x,y);
    
    NSLog(@"%@", [NSString stringWithFormat: @"x=%d, y=%d, result=%d",x,y,output]);
    
    CDVPluginResult* result = [CDVPluginResult
                               resultWithStatus:CDVCommandStatus_OK
                               messageAsInt:output];
    
    [self.commandDelegate sendPluginResult:result callbackId:command.callbackId];
}

@end
