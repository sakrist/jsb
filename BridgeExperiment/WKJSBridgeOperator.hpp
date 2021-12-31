//
//  WKJSBridgeOperator.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 29/12/2021.
//

#ifndef WKJSBridgeOperator_hpp
#define WKJSBridgeOperator_hpp

#import <WebKit/WebKit.h>
#include <stdio.h>
#include "JSBridge.hpp"

#include "json.hpp"
using json = nlohmann::json;


class WKJSBridgeOperator : public JSBridgeOperator {
public:
    WKJSBridgeOperator(WKWebView *view);
    
    void send(const char* message) const override;
    
    void recive(const char* message) const override {
        
        auto json = json::parse(message);
        std::cout << json << std::endl;
        
        // TODO: subclass and implement here JSInvokeMessage
        
        std::string callback = json["callback"].is_null() ? "" : json["callback"];
        std::string callback_id = json["callback_id"].is_null() ? "" : json["callback_id"];
        
        JSInvokeMessage me = { json["object"], json["function"], callback, callback_id };
        JSBridge::getInstance().recive(me);
    }
    
    
public:
    
    WKWebView *_view;
};

#endif /* WKJSBridgeOperator_hpp */
