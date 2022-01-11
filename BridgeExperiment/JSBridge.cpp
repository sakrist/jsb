//
//  JSBridge.cpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 25/12/2021.
//

#include "JSBridge.hpp"

using namespace jsbridge;

 JSBridge& JSBridge::getInstance() {
    static JSBridge instance;
    return instance;
}

//JSBridge_BINDINGS(default) {
//    class_<std::string>("string")
//    .function("c_str", &std::string::c_str);
//}
