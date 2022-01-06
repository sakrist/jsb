//
//  WKJSBridgeCommunicator.cpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 29/12/2021.
//

#include "WKJSBridgeCommunicator.hpp"

WKJSBridgeCommunicator::WKJSBridgeCommunicator(WKWebView *view) {
    _view = view;
}

void WKJSBridgeCommunicator::eval(const char* js_code, std::optional<std::function<void(const char*)>>&& completion) const {
#if DEBUG
    std::cout << "eval: " << js_code << std::endl;
#endif
    NSString *string = [NSString stringWithCString:js_code encoding:NSUTF8StringEncoding];
    
    [_view evaluateJavaScript:string completionHandler:^(NSString* _Nullable result, NSError * _Nullable error) {
        if (result && completion.has_value()) {
            completion.value()(result.UTF8String);
        }
        if (error) {
            NSLog(@"%@", error);
        }
    }];
}
