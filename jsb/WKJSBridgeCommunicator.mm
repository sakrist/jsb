//
//  jsb
//  Created by Volodymyr Boichentsov on 29/12/2021.
//

#include "WKJSBridgeCommunicator.hpp"

WKJSBridgeCommunicator::WKJSBridgeCommunicator(WKWebView *view) {
    _view = view;
}

void WKJSBridgeCommunicator::eval(const char* js_code, std::function<void(const char*)>&& completion) const {
#if DEBUG
    std::cout << "eval: " << js_code << std::endl;
#endif
    NSString *string = [NSString stringWithCString:js_code encoding:NSUTF8StringEncoding];
    
    __block auto bcompletion = completion; 
    [_view evaluateJavaScript:string completionHandler:^(NSString* _Nullable result, NSError * _Nullable error) {
        if (result && bcompletion) {
            if ([result isKindOfClass:NSDictionary.class]) {
                // TODO: do something with it.
#if DEBUG
                NSLog(@"result dictionary %@", result);
#endif
            } else if ([result isKindOfClass:NSString.class]) {
#if DEBUG
                NSLog(@"result %@", result);
#endif
                bcompletion(result.UTF8String);
            }
        }
        if (error) {
            NSLog(@"%@", error);
        }
    }];
}
