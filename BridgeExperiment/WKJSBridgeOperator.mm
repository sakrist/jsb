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
    NSString *string = [NSString stringWithCString:message encoding:NSUTF8StringEncoding];
    [_view evaluateJavaScript:string completionHandler:^(id _Nullable, NSError * _Nullable error) {
        NSLog(@"%@", error);
    }];
}
