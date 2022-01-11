//
//  WKJSBridgeCommunicator.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 29/12/2021.
//

#pragma once

#import <WebKit/WebKit.h>
#include <stdio.h>
#include "JSBridge.hpp"

#include "json.hpp"
using json = nlohmann::json;


class WKJSBridgeCommunicator : public jsbridge::JSBridgeCommunicator {
public:
    WKJSBridgeCommunicator(WKWebView *view);
    
    void eval(const char* js_code, std::function<void(const char*)> completion) const override;
    
    void recive(const char* message, std::function<void(const char*)> completion) const override {
        
        auto json = json::parse(message);
#if DEBUG
        std::cout << "recive: " << json << std::endl;
#endif
        
        std::string callback = json["callback"].is_null() ? "" : json["callback"];
        std::string callback_id = json["cid"].is_null() ? "" : json["cid"];
        uintptr_t ptr = json["object"].is_null() ? 0 : json["object"].get<uintptr_t>();
        
        std::string classid = json["class"].is_null() ? "" : json["class"];
        
        jsbridge::JSInvokeMessage msgStruct = { ptr, std::move(classid), json["function"],
            std::move(callback), std::move(callback_id) };
        
        msgStruct.completion = completion;
        
        auto& args = json["args"];
        if (!args.is_null() && args.is_array() && args.size() > 0) {
        
//            std::stringstream ss("", std::ios_base::app |std::ios_base::out);
//            ss << json["args"];
//            msgStruct.argss = ss.str();
            
            // TODO: read types from function!!
            msgStruct.args[0] = args[0].get<int>();
            if (args.size() > 1) {
                msgStruct.args[1] = args[1].get<int>();
            }
        }
        
        jsbridge::JSBridge::getInstance().recive(msgStruct);
    }
    
    
public:
    
    WKWebView *_view;
};

