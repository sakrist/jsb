//
//  WKJSBridgeOperator.cpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 29/12/2021.
//

#include "WKJSBridgeOperator.hpp"

WKJSBridgeOperator::WKJSBridgeOperator(WKWebView *view) {
    _view = view;
}

void WKJSBridgeOperator::send(const char* message) const {
    std::cout << "send: " << message << std::endl;
    NSString *string = [NSString stringWithCString:message encoding:NSUTF8StringEncoding];
    
//    [_view callAsyncJavaScript:<#(nonnull NSString *)#> arguments:<#(nullable NSDictionary<NSString *,id> *)#> inFrame:<#(nullable WKFrameInfo *)#> inContentWorld:<#(nonnull WKContentWorld *)#> completionHandler:<#^(id _Nullable, NSError * _Nullable error)completionHandler#>]
    
    [_view evaluateJavaScript:string completionHandler:^(id _Nullable, NSError * _Nullable error) {
        if (error) {
            NSLog(@"%@", error);
        }
    }];
}
